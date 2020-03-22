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

Interface *DistanceVectorRouting::Route(ProtocolPacket &packet) {
  auto search = table_.find(packet.get_destination_address());
  if (search == table_.end() || search->second.metrics >= MAX_METRICS) {
    return nullptr;
  }
  Interface *iface = search->second.via_interface;

  // This does no longer have to hold since routing updates and periodic
  // interface updates are not synced.
  // assert(iface->IsConnected());

  return iface;
}

bool DistanceVectorRouting::Process(ProtocolPacket &packet,
                                    Interface *processing_interface) {
  assert(packet.is_routing_update());
  Statistics::RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  bool change_occured =
      UpdateRouting(update_packet.table_copy, processing_interface);
  if (change_occured) {
    CheckPeriodicUpdate();
  }
  return false;
}

void DistanceVectorRouting::Init() {
  for (auto &iface : node_.get_active_interfaces()) {
    if (&iface->get_other_end_node() == &node_) {
      table_.emplace(iface->get_other_end_node().get_address(),
                     Record(iface.get(), 0));  // metrics 0 for this node
    } else {
      table_.emplace(iface->get_other_end_node().get_address(),
                     Record(iface.get(), 1));  // metrics 1 for direct neighbor
    }
  }
}

void DistanceVectorRouting::UpdateInterfaces() {
  // Search routing table for invalid records.
  for (auto &pair : table_) {
    if (pair.second.via_interface &&
        !pair.second.via_interface->IsConnected()) {
      pair.second.via_interface = nullptr;
      pair.second.metrics = MAX_METRICS;
    }
  }
  // Now delete not connected interfaces from node_.
  for (auto it = node_.get_active_interfaces().begin();
       it != node_.get_active_interfaces().end();) {
    if (!(*it)->IsConnected()) {
      it = node_.get_active_interfaces().erase(it);
    } else {
      ++it;
    }
  }
}

void DistanceVectorRouting::Update() {
  for (auto &interface : node_.get_active_interfaces()) {
    // Skip over reflexive interfaces.
    if (&interface->get_other_end_node() == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet = std::make_unique<DVRoutingUpdate>(
        node_.get_address(), interface->get_other_end_node().get_address(),
        table_);
    // Register to statistics before we move.
    Statistics::RegisterRoutingOverheadSend();
    Statistics::RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

DistanceVectorRouting::Record::Record(Interface *via_interface,
                                      uint32_t metrics)
    : via_interface(via_interface), metrics(metrics) {}

bool DistanceVectorRouting::UpdateRouting(
    const DistanceVectorRouting::RoutingTableType &update,
    Interface *processing_interface) {
  bool changed = false;
  // Neighbor interface should be in table_ form Init() unless interfaces
  // changed due to node movement.
  auto neighbor_record =
      table_.find(processing_interface->get_other_end_node().get_address());
  if (neighbor_record == table_.end()) {
    // Change did not happen but Update is needed.
    return true;
  }

  uint32_t distance_to_neighbor = neighbor_record->second.metrics;
  // Check whether the other table has some more efficient route
  for (auto &pair : update) {
    // Skip invalid records.
    if (pair.second.via_interface == nullptr) {
      continue;
    }
    uint32_t combined_metrics = distance_to_neighbor + pair.second.metrics;
    auto match = table_.find(pair.first);
    if (match == table_.end()) {
      // If there is no record of such to_address add a new record to the table
      table_.emplace(pair.first,
                     Record(processing_interface, combined_metrics));
      changed = true;
    } else if (match->second.via_interface ==
                   &pair.second.via_interface->get_other_end() &&
               match->second.metrics != pair.second.metrics) {
      // If the shortest route goes through that iterface update the value to
      // neighbor interface metrics.
      match->second.metrics = combined_metrics;
      changed = true;
    } else if (match->second.metrics > combined_metrics) {
      // If combined metrics is less than current value change it
      match->second.via_interface = processing_interface;
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
