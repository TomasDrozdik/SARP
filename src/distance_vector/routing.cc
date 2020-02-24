//
// routing.cc
//

#include "routing.h"

#include <algorithm>
#include <cassert>

#include "update_packet.h"
#include "../structure/event.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) { }

Interface *DistanceVectorRouting::Route(ProtocolPacket &packet) {
  auto search = table_.find(packet.get_destination_address());
  if (search == table_.end() || search->second.metrics >= MAX_METRICS) {
    return nullptr;
  }
  if (!search->second.via_interface)
    assert(search->second.via_interface->IsConnected());
  return search->second.via_interface;
}

bool DistanceVectorRouting::Process(ProtocolPacket &packet,
    Interface *processing_interface) {
  Simulation::get_instance().get_statistics().
      RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  bool change_occured = UpdateRouting(update_packet.table_copy,
      processing_interface);

  Time current_time = Simulation::get_instance().get_current_time();
  if (change_occured) {
    Time due_update = current_time - last_update_;
    if (due_update > update_period_) {
      last_update_ = current_time;
      // Do an instantanious Update() without UpdateEvent.
      Update();
    } else {
      // Plan the update at the given period.
      Time time_to_period = current_time % update_period_;
      last_update_ = current_time + time_to_period;
      Simulation::get_instance().ScheduleEvent(
          std::make_unique<UpdateRoutingEvent>(last_update_, true, *this));
    }
  }
  return false;
}

void DistanceVectorRouting::Init() {
  for (auto &iface : node_.get_active_interfaces()) {
    if (&iface->get_other_end_node() == &node_) {
      table_.emplace(
          iface->get_other_end_node().get_address()->Clone(),
          Record(iface.get(), 0));
    } else {
      table_.emplace(
          iface->get_other_end_node().get_address()->Clone(),
          Record(iface.get(), 1));
    }
  }
}

void DistanceVectorRouting::UpdateInterfaces() {
  // Search routing table for invalid records.
  for (auto &record : table_) {
    if (record.second.via_interface &&
        !record.second.via_interface->IsConnected()) {
      record.second.via_interface = nullptr;
      record.second.metrics = MAX_METRICS;
    }
  }
  // Now delete not connected interfaces from node_.
  for (auto it = node_.get_active_interfaces().begin();
      it != node_.get_active_interfaces().end(); ) {
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
        node_.get_address()->Clone(),
        interface->get_other_end_node().get_address()->Clone(), table_);
    // Add to statistics before we move unique_ptr<Packet>
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSend();
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSize(
        packet->get_size());
    // Schedule immediate send.
    // TODO: make immediate event creation without constant -> separate funciton
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, false, node_, std::move(packet)));
  }
}

DistanceVectorRouting::Record::Record(Interface *via_interface,
    uint32_t metrics) : via_interface(via_interface), metrics(metrics) { }

bool DistanceVectorRouting::UpdateRouting(
    const DistanceVectorRouting::RoutingTableType &update,
    Interface *processing_interface) {
  bool changed = false;
  // Neighbor interface should be in table_ form Init() unless interfaces
  // changed due to node movement.
  auto neighbor_record = table_.find(processing_interface->get_other_end_node().
      get_address());
  if (neighbor_record == table_.end()) {
    // Change did not happen but Update is needed.
    return false;
  }

  uint32_t distance_to_neighbor = neighbor_record->second.metrics;
  // Check whether the other table has some more efficient route
  for (auto &record : update) {
    if (record.second.via_interface == nullptr) {
      continue;
    }
    auto match = table_.find(record.first);
    uint32_t combined_metrics = distance_to_neighbor + record.second.metrics;
    if (match == table_.end()) {
      // If there is no record of such to_address add a new record to the table
      table_.emplace(record.first->Clone(), Record(processing_interface,
          combined_metrics));
      changed = true;
    } else if (match->second.via_interface ==
        &record.second.via_interface->get_other_end() &&
            match->second.metrics != record.second.metrics) {
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
