//
// dv_routing.cc
//

#include "dv_routing.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) { }

Interface * const DistanceVectorRouting::Route(const Address &address) const {
	return routing_table_.Route(address);
}

void DistanceVectorRouting::Init() {
	auto ifaces = node_.get_active_connections();
	for (auto &iface : ifaces) {
		SimpleAddress other_end_address(dynamic_cast<SimpleAddress&>(
				*iface.get_other_end()->get_address()));
		routing_table_.Init(node_.get_active_connections());
	}
}

void DistanceVectorRouting::RoutingTable::Init(
		const std::vector<Interface> &active_interfaces) {
	for (auto &iface : active_interfaces_) {
		table_.emplace(
			// SimpleAddress to_address
			dynamic_cast<SimpleAddress &>(*iface.get_other_end()->get_address().get()),
			// Record empty default initialized with interfaces on 1 hop
			active_interfaces_, &iface, 1);
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
	SimpleAddress other_address(dynamic_cast<SimpleAddress &>(
		*processing_interface->get_other_end()->get_address()));
	// Go over all addresses in other table
	for (auto it : other.table_) {
		auto to_address_it = table_.find(it.first);
		auto combined_metrics =
				table_[other_address].min_metrics + it.second.min_metrics;
		if (to_address_it == table_.end()) {
			// If there is no record of such to_address add a new record to the table
			table_.emplace(it.first,
					// New record for address it.first with combined metrics
					active_interfaces_, processing_interface, combined_metrics);
			changed = true;
		} else if (table_[it.first].data[processing_interface] >
				combined_metrics) {
			// If combined metrics is less than current value change it
			table_[it.first].data[processing_interface] = combined_metrics;
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

}  // namespace simulation
