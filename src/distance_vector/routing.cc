//
// routing.cc
//

#include "distance_vector/routing.h"

#include <algorithm>
#include <cassert>

#include "distance_vector/update_packet.h"
#include "structure/event.h"
#include "structure/simulation.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) {}

Node *DistanceVectorRouting::Route(Packet &packet) {
  // Find a matching record.
  const auto it = table_.find(packet.get_destination_address());
  if (it != table_.end()) {
    assert(it->second.second != &node_);
    return it->second.second;
  } else {
    return nullptr;
  }
}

void DistanceVectorRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured =
        UpdateRouting(update_packet.update, from_node, env.stats);
    if (change_occured) {
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void DistanceVectorRouting::Init(Env &env) {
  // Add this node addresses at distance 0.
  const auto [it, success] = table_.insert({node_.get_address(), {0, &node_}});
  assert(success);
  // Neighbors were already added in UpdateNeighbors.
  // So just begin the periodic routing update.
  CheckPeriodicUpdate(env);
}

void DistanceVectorRouting::UpdateNeighbors(uint32_t connection_range) {
  // Search routing table for invalid records.
  for (auto it = table_.cbegin(); it != table_.end(); /* no increment */) {
    Node *neighbor = it->second.second;
    if (node_.IsConnectedTo(*neighbor, connection_range)) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = table_.erase(it);
    }
  }
  // Now add new neighbors at 1 hop distance.
  for (Node *neighbor : node_.get_neighbors()) {
    // Skip over this node.
    if (neighbor == &node_) {
      continue;
    }
    const auto [it, success] = table_.insert({neighbor->get_address(), {1, neighbor}});
    if (!success) {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.first = 1;
    }
  }
}

void DistanceVectorRouting::Update(Env &env) {
  CreateUpdateMirror();
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<Packet> packet = std::make_unique<DVRoutingUpdate>(
        node_.get_address(), neighbor->get_address(), mirror_id_,
        update_mirror_);
    // Register to statistics before we move packet away.
    env.stats.RegisterRoutingOverheadSend();
    env.stats.RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    env.simulation.ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

bool DistanceVectorRouting::AddRecord(UpdateTable::const_iterator update_it, Neighbor *via_neighbor) {
  constexpr Metrics distance_to_neighbor = 1;
  bool changed = false;
  const auto &[address, cost] = *update_it;
  Metrics actual_cost = cost + distance_to_neighbor;
  auto [it, success] = table_.insert({address, {actual_cost, via_neighbor}});
  if (!success) {
    // There is already an element with given address.
    // Check which neighbor it goes through.
    if (it->second.second == via_neighbor) {
      // Just asign new cost
      it->second.first = actual_cost;
      changed = true;
    } else {
      if (it->second.first > actual_cost) {
        // Replace the record for given address.
        it->second.first = actual_cost;
        it->second.second = via_neighbor;
        changed = true;
      }
    }
  }
  return changed;
}

bool DistanceVectorRouting::UpdateRouting(
    const UpdateTable &update, Neighbor *from_node,
    Statistics &stats) {
  bool changed = false;
  for (auto it = update.cbegin(); it != update.cend(); ++it) {
    if (AddRecord(it, from_node)) {
      changed = true;
    }
  }
  return changed;
}

void DistanceVectorRouting::CreateUpdateMirror() {
  ++mirror_id_;
  update_mirror_.clear();
  for (const auto &[address, cost_neighbor_pair] : table_) {
    auto [it, success] =
        update_mirror_.emplace(address, cost_neighbor_pair.first);
    assert(success);
  }
}

}  // namespace simulation
