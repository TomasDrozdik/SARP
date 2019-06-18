//
// dv_routing.cc
//

#include "dv_routing.h"

#include <algorithm>
#include <cassert>

#include "dv_packet.h"
#include "../structure/event.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) { }

Interface *DistanceVectorRouting::Route(ProtocolPacket &packet) {
  auto search = table_.find(packet.get_destination_address());
  if (search == table_.end() || search->second.metrics >= MAX_METRICS) {
    return nullptr;
  }
  return search->second.via_interface;
}

bool DistanceVectorRouting::Process(ProtocolPacket &packet,
    Interface *processing_interface) {
  Simulation::get_instance().get_statistics().
      RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  bool change_occured = UpdateRouting(update_packet.table_copy,
      processing_interface);
  if (change_occured) {
    Update();
  }
  return false;
}

void DistanceVectorRouting::Init() {
  active_interfaces_routingPOV_ = node_.get_active_connections();
  for (auto &iface : active_interfaces_routingPOV_) {
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
  // Find interfaces which correspond (operator==) to original interfaces and
  // store them in following map.
  std::unordered_map<Interface *, Interface *> same_interfaces;

  for (auto &iface : active_interfaces_routingPOV_) {
    auto it = std::find_if(node_.get_active_connections().begin(),
        node_.get_active_connections().end(),
        [&iface](const std::shared_ptr<Interface> &other) {
          return *iface == *other;
        });
    if (it != node_.get_active_connections().end()) {
      same_interfaces[iface.get()] = it->get();
    }
  }

  // Now update the records according to corresponding new interfaces previously
  // found.
  for (auto &record : table_) {
    auto match = same_interfaces.find(record.second.via_interface);
		if (match != same_interfaces.end())  {
			record.second.via_interface = match->second;
      // metrics remains
		} else {
      // Invalidate record, leave it here for future.
			record.second.via_interface = nullptr;
			record.second.metrics = MAX_METRICS;
    }
  }

  // Now that all pointers in records are renewed away all connections.
  active_interfaces_routingPOV_ = node_.get_active_connections();
}

void DistanceVectorRouting::Update() {
  for (auto &interface : active_interfaces_routingPOV_) {
    // Skip over reflexive interfaces.
    if (&interface->get_other_end_node() == &node_) {
      continue;
    }
    // Create update packet.
    auto packet = std::make_unique<DVRoutingUpdate>(
        node_.get_address()->Clone(),
        interface->get_other_end_node().get_address()->Clone(), table_);
    // Add to statistics before we move unique_ptr<Packet>
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSend();
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSize(
        packet->get_size());
    // Schedule immediate send.
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