//
// routing.cc
//

#include "distance_vector/routing.h"

#include <algorithm>
#include <cassert>

#include "distance_vector/update_packet.h"
#include "structure/event.h"
#include "structure/simulation.h"
#include "structure/statistics.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) {}

Node *DistanceVectorRouting::Route(ProtocolPacket &packet) {
  auto search = table_.find(packet.get_destination_address());
  if (search == table_.end() || search->second.metrics >= MAX_METRICS) {
    return nullptr;
  }
  return search->second.via_node;
}

void DistanceVectorRouting::Process(ProtocolPacket &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  Statistics::RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  if (update_packet.original_mirror_id == update_packet.mirror_id) {
    bool change_occured = UpdateRouting(update_packet.mirror_table, from_node);
    if (change_occured) {
      CheckPeriodicUpdate();
    }
  } else {
    Statistics::RegisterInvalidRoutingMirror();
  }
}

void DistanceVectorRouting::Init() {
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      // Create a 0 metrics record for this node.
      table_.emplace(neighbor->get_address(), Record(neighbor, 0));
    } else {
      // Create a 1 metrics record for a neighbor.
      table_.emplace(neighbor->get_address(), Record(neighbor, 1));
    }
  }
  // Now begin the periodic routing update.
  CheckPeriodicUpdate();
}

void DistanceVectorRouting::UpdateNeighbors() {
  // Search routing table for invalid records.
  for (auto &pair : table_) {
    if (pair.second.via_node && !node_.IsConnectedTo(*pair.second.via_node)) {
      pair.second.via_node = nullptr;
      pair.second.metrics = MAX_METRICS;
    }
  }
}

void DistanceVectorRouting::Update() {
  // Create new mirro update table.
  ++mirror_id_;
  mirror_table_ = table_;
  for (auto neighbor : node_.get_neighbors()) {
    // Skip over this node.
    if (neighbor == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet = std::make_unique<DVRoutingUpdate>(
        node_.get_address(), neighbor->get_address(), mirror_id_,
        mirror_table_);
    // Register to statistics before we move packet away.
    Statistics::RegisterRoutingOverheadSend();
    Statistics::RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

DistanceVectorRouting::Record::Record(Node *via_node, uint32_t metrics)
    : via_node(via_node), metrics(metrics) {}

bool DistanceVectorRouting::UpdateRouting(
    const DistanceVectorRouting::RoutingTableType &update, Node *from_node) {
  bool changed = false;
  // Neighbor should be in table_ form Init() unless neighbors changed due to
  // node movement.
  auto neighbor_record = table_.find(from_node->get_address());
  if (neighbor_record == table_.end()) {
    // Change did not happen but Update is needed since neighbor isn't in
    // routing table.
    return true;
  }

  uint32_t distance_to_neighbor = neighbor_record->second.metrics;
  // Check whether the other table has some more efficient route
  for (auto &pair : update) {
    // Skip invalid records.
    if (pair.second.via_node == nullptr) {
      continue;
    }
    uint32_t combined_metrics = distance_to_neighbor + pair.second.metrics;
    auto match = table_.find(pair.first);
    if (match == table_.end()) {
      // If there is no record of such to_address add a new record to the table
      table_.emplace(pair.first, Record(from_node, combined_metrics));
      changed = true;
    } else if (match->second.via_node == pair.second.via_node &&
               match->second.metrics != pair.second.metrics) {
      // If the shortest route goes through the same neighbor, update the value
      // to neighbor metrics.
      match->second.metrics = combined_metrics;
      changed = true;
    } else if (match->second.metrics > combined_metrics) {
      // If combined metrics is less than current value change it
      match->second.via_node = from_node;
      match->second.metrics = combined_metrics;
      changed = true;
    }
    if (combined_metrics >= MAX_METRICS) {
      // Count to infinity threshold
      return false;
    }
  }
  return changed;
}

}  // namespace simulation
