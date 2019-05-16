//
// dv_routing.cc
//

#include "dv_routing.h"

#include <cassert>

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) :
		Routing(node), routing_table_(node.get_active_connections()) { }

Interface * const DistanceVectorRouting::Route(const Address &address) const {
	return routing_table_.Route(address);
}

void DistanceVectorRouting::Init() {
	auto ifaces = node_.get_active_connections();
	for (auto &iface : ifaces) {
		SimpleAddress other_end_address(dynamic_cast<SimpleAddress&>(
				*iface.get_other_end()->get_address()));
		routing_table_.Init();
	}
}

DistanceVectorRouting::RoutingTable::RoutingTable(
	std::vector<Interface> &active_interfaces) :
			active_interfaces_(active_interfaces) { }

void DistanceVectorRouting::RoutingTable::Init() {
	for (auto &iface : active_interfaces_) {
		table_.emplace(
			SimpleAddress(dynamic_cast<SimpleAddress &>(
					*iface.get_other_end()->get_address().get())),
			Record(active_interfaces_, &iface, 1U));
	}
}

Interface * const DistanceVectorRouting::RoutingTable::Route(
		const Address &to_address) const {
	auto to_address_hit = table_.find(
		dynamic_cast<const SimpleAddress&>(to_address));
	if (to_address_hit == table_.end()) {
		return nullptr;
	}
	return to_address_hit->second.goto_interface;
}

bool DistanceVectorRouting::RoutingTable::MergeRoutingTables(
		const RoutingTable& other, Interface *processing_interface) {
	bool changed = false;
	// Find the distance in given metrics to other node to calculate the combined
	// metrics later according to Bellman-Ford algorithm
	SimpleAddress other_address(dynamic_cast<SimpleAddress &>(
		*processing_interface->get_other_end()->get_address()));
  // Neighbor should be interface should be in table_ form Init()
	auto other_record = table_.find(other_address);
	assert(other_record != table_.end());
	uint distance_to_other = other_record->second.min_metrics;

	// Go over all addresses in other table
	for (auto &it : other.table_) {
		auto to_address_it = table_.find(it.first);
		uint combined_metrics = distance_to_other + it.second.min_metrics;
		if (to_address_it == table_.end()) {
			// If there is no record of such to_address add a new record to the table
			table_.emplace(
					SimpleAddress(it.first),
					RoutingTable::Record(
							active_interfaces_, processing_interface, combined_metrics));
			changed = true;
		} else if (table_[it.first].data_[processing_interface] >
				combined_metrics) {
			// If combined metrics is less than current value change it
			table_[it.first].data_[processing_interface] = combined_metrics;
			changed = true;
			if (table_[it.first].min_metrics > combined_metrics) {
				// If the overall metrics minimum changed apply it
				table_[it.first].min_metrics = combined_metrics;
				table_[it.first].goto_interface = processing_interface;
			}
		}
	}
	return changed;
}

DistanceVectorRouting::RoutingTable::Record::Record(
		std::vector<Interface> &interfaces, Interface *from_interface,
		uint from_interface_metrics) {
	for (auto &interface : interfaces) {
		if (&interface == from_interface) {
			data_[&interface] = from_interface_metrics;
		} else {
			data_[&interface] = UINT_MAX;
		}
	}
	this->min_metrics = from_interface_metrics;
	this->goto_interface = from_interface;
}

}  // namespace simulation