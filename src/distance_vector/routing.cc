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
  change_occured_ = UpdateRouting(update_packet.RetrieveUpdate(), from_node);
  if (change_occured_) {
    CreateUpdateMirror();
    NotifyChange();
  }
}

void DistanceVectorRouting::Init(Env &env) {
  UpdateAddresses();
  CheckPeriodicUpdate(env);
}

void DistanceVectorRouting::SendUpdate(Env &env, Node *neighbor) {
  // Create update packet.
  std::unique_ptr<Packet> packet = std::make_unique<DVRoutingUpdate>(
      node_.get_address(), neighbor->get_address(), update_mirror_);
  // Register to statistics before we move packet away.
  env.stats.RegisterRoutingOverheadSend();
  env.stats.RegisterRoutingOverheadSize(packet->get_size());
  // Schedule immediate recieve on neighbor to bypass Node::Send() which calls
  // Routing::Route which is not desired.
  env.simulation.ScheduleEvent(std::make_unique<RecvEvent>(
      1, TimeType::RELATIVE, node_, *neighbor, std::move(packet)));
}

void DistanceVectorRouting::UpdateAddresses() {
  for (const auto& address : node_.get_addresses()) {
    auto [record, success] = table_.insert({address,
        {.cost = MIN_COST, .via_node = &node_}});
    if (success == false) {
      record->second.cost = MIN_COST;
    }
  }
  change_occured_ = true;
  CreateUpdateMirror();
}

void DistanceVectorRouting::UpdateNeighbors(Env &env,
    const std::set<Node *> &current_neighbors) {
  // Search routing table for invalid records.
  for (auto it = table_.cbegin(); it != table_.end(); /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_general().connection_range)) {
      assert(current_neighbors.contains(neighbor));
      ++it;
    } else {
      assert(current_neighbors.contains(neighbor) == false);
      it = table_.erase(it);
    }
  }
  // If there are new neighbors set change_occured for nech CheckPeriodicUpdate.
  const auto &old_neighbors = node_.get_neighbors();
  for (const auto current_neighbor : current_neighbors) {
    if (old_neighbors.contains(current_neighbor) == false) {
      change_occured_ = true;
      break;
    }
  }
}

bool DistanceVectorRouting::AddRecord(UpdateTable::const_iterator update_it,
                                      Node *via_neighbor) {
  const auto &[address, cost] = *update_it;
  Cost actual_cost = cost + NEIGHBOR_COST;
  auto [it, success] = table_.insert({address, {actual_cost, via_neighbor}});
  if (success) {
    return true;
  }
  // There is already an element with given address.
  // Check which neighbor it goes through.
  if (it->second.via_node == via_neighbor) {
    // If it goes through the same neighbor costs should match.
    if (it->second.cost != actual_cost) {
      // Otherwise record that this route has changed its cost.
      it->second.cost = actual_cost;
      return true;
    }
  } else {
    // If it goes through different neighbor pick a better route one.
    if (it->second.cost > actual_cost) {
      it->second = {.cost = actual_cost, .via_node = via_neighbor};
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
  update_mirror_.clear();
  for (const auto &[address, cost_neighbor_pair] : table_) {
    auto [it, success] =
        update_mirror_.emplace(address, cost_neighbor_pair.cost);
    assert(success);
  }
}

}  // namespace simulation
