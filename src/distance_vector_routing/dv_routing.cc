//
// dv_routing.cc
//

#include "dv_routing.h"

#include <limits>
#include <cassert>

#include "dv_packet.h"
#include "../structure/event.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node),
		active_interfaces_shared_copy_(node_.get_active_connections()) { }

Interface * DistanceVectorRouting::Route(ProtocolPacket &packet) {
	auto search = table_.find(packet.get_destination_address());
	if (search == table_.end()) {
		return nullptr;
	}
	return search->second.goto_interface;
}

bool DistanceVectorRouting::Process(ProtocolPacket &packet,
		Interface *processing_interface) {
	Simulation::get_instance().get_statistics().
			RegisterRoutingOverheadDelivered();

	DVRoutingUpdate &other_routing = dynamic_cast<DVRoutingUpdate &>(packet);
	bool change_occured = UpdateRouting(other_routing.routing_update,
			processing_interface);
	if (change_occured) {
		Update();
	}
	return false;
}

void DistanceVectorRouting::Init() {
	for (auto &iface : node_.get_active_connections()) {
		table_.emplace(iface->get_other_end_node().get_address()->Clone(),
			Record(active_interfaces_shared_copy_, iface.get(), 1));
	}
	
}

void DistanceVectorRouting::Update() {
	for (auto &interface : node_.get_active_connections()) {
		// Create update packet.
		auto packet = std::make_unique<DVRoutingUpdate>(
				node_.get_address()->Clone(),
				interface->get_other_end_node().get_address()->Clone(), *this);
		// Add to statistics before we move unique_ptr<Packet>
		Simulation::get_instance().get_statistics().RegisterRoutingOverheadSend();
		Simulation::get_instance().get_statistics().RegisterRoutingOverheadSize(
			packet->get_size());
		// Schedule immediate send.
		Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
				0, false, node_, std::move(packet)));
	}
}

DistanceVectorRouting::Record::Record(
		const std::vector<std::shared_ptr<Interface>> &interfaces,
		Interface *from_interface, uint32_t from_interface_metrics) {
	for (auto &interface : interfaces) {
		if (interface.get() == from_interface) {
			data_[interface.get()] = from_interface_metrics;
		} else {
			data_[interface.get()] = std::numeric_limits<uint32_t>::max();
		}
	}
	this->min_metrics = from_interface_metrics;
	this->goto_interface = from_interface;
}

bool DistanceVectorRouting::UpdateRouting(
		const DistanceVectorRouting& other, Interface *processing_interface) {
	bool changed = false;
	// Find the distance in given metrics to other node to calculate the combined
	// metrics later according to Bellman-Ford algorithm
	auto &other_address =
			processing_interface->get_other_end_node().get_address();
  // Neighbor interface should be in table_ form Init() unless interfaces
	// changed due to node movement.
	auto other_record = table_.find(other_address);
	if (other_record != table_.end()) {
		// Change did not happen but Update is needed.
		return false;
	}
	uint32_t distance_to_other = other_record->second.min_metrics;

	// Check whether the other table has some more efficient routes
	for (auto &it : other.table_) {
		auto search = table_.find(it.first);
		uint32_t combined_metrics = distance_to_other + it.second.min_metrics;
		if (search == table_.end()) {
			// If there is no record of such to_address add a new record to the table
			table_.emplace(it.first->Clone(),
					Record(node_.get_active_connections(), processing_interface,
							combined_metrics));
			changed = true;
		} else if (search->second.data_[processing_interface] >
				combined_metrics) {
			// If combined metrics is less than current value change it
			search->second.data_[processing_interface] = combined_metrics;
			changed = true;
			if (search->second.min_metrics > combined_metrics) {
				// If the overall metrics minimum changed apply it
				search->second.min_metrics = combined_metrics;
				search->second.goto_interface = processing_interface;
			}
		}
	}
	return changed;
}

}  // namespace simulation