//
// routing.cc
//

#include "distance_vector/routing.h"

#include <cassert>

#include "distance_vector/update_packet.h"
#include "structure/event.h"
#include "structure/simulation.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) {}

Node *DistanceVectorRouting::Route(Env &, Packet &packet) {
  // Find a matching record.
  const auto it = table_.find(packet.get_destination_address());
  if (it != table_.end()) {
    assert(it->second.via_node != &node_);
    return it->second.via_node;
  } else {
    return nullptr;
  }
}

void DistanceVectorRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured = UpdateRouting(update_packet.update, from_node);
    if (change_occured) {
      change_occured_ = true;
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void DistanceVectorRouting::Init(Env &env) {
  // Add this node addresses at distance 0.
  const auto [it, success] = table_.insert(
      {node_.get_address(), {.metrics = MIN_METRICS, .via_node = &node_}});
  assert(success);
  // Neighbors were already added in UpdateNeighbors.
  // So just begin the periodic routing update.
  CheckPeriodicUpdate(env);
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

void DistanceVectorRouting::UpdateNeighbors(Env &env) {
  // Search routing table for invalid records.
  for (auto it = table_.cbegin(); it != table_.end(); /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_connection_range())) {
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
    const auto [it, success] =
        table_.insert({neighbor->get_address(),
                       {.metrics = NEIGHBOR_METRICS, .via_node = neighbor}});
    if (!success) {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.metrics = NEIGHBOR_METRICS;
    }
  }
}

bool DistanceVectorRouting::AddRecord(UpdateTable::const_iterator update_it,
                                      Node *via_neighbor) {
  const auto &[address, metrics] = *update_it;
  Metrics actual_metrics = metrics + NEIGHBOR_METRICS;
  auto [it, success] = table_.insert({address, {actual_metrics, via_neighbor}});
  if (success) {
    return true;
  }
  // There is already an element with given address.
  // Check which neighbor it goes through.
  if (it->second.via_node == via_neighbor) {
    // If it goes through the same neighbor costs should match.
    if (it->second.metrics != actual_metrics) {
      // Otherwise record that this route has changed its metrics.
      it->second.metrics = actual_metrics;
      return true;
    }
  } else {
    // If it goes through different neighbor pick a better route one.
    if (it->second.metrics > actual_metrics) {
      it->second = {.metrics = actual_metrics, .via_node = via_neighbor};
      return true;
    }
  }
  return false;
}

bool DistanceVectorRouting::UpdateRouting(const UpdateTable &update,
                                          Node *from_node) {
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
  for (const auto &[address, metrics_neighbor_pair] : table_) {
    auto [it, success] =
        update_mirror_.emplace(address, metrics_neighbor_pair.metrics);
    assert(success);
  }
}

}  // namespace simulation
