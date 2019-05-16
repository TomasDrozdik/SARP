//
// dv_routing.h
//

#ifndef SARP_SRC_SIMULATION_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
#define SARP_SRC_SIMULATION_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_

#include <functional>
#include <vector>
#include <unordered_map>
#include <climits>

#include "../structure/address.h"
#include "../structure/interface.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"


namespace simulation {

class DistanceVectorRouting final : public Routing {
 public:
	DistanceVectorRouting(Node &node);
	Interface * const Route(const Address &addr) const override;
	void Init() override;
 private:
	class RoutingTable {
	 public:
	  RoutingTable() = default;
	  // Initializes the routing table with the set of interfaces on a given node
		// All the active interafaces would be at a 1 hop distance
		void Init(const std::vector<Interface> &active_interfaces);

		// Finds metrics defined shortest route to to_address
		Interface * const Route(const Address &to_address) const;

		// Updates this with information form other RoutingTable incomming from
		// processing interface
		bool MergeRoutingTables(const RoutingTable& other,
				Interface *processing_interface);
	 private:
	  // Record in the routing table assigned to given address
		// Used instead of tuple for better readability and default initialization
		// with ctor
		struct Record {
			// Creates new Record fills all interfaces in data and initializes yet
			// unknow with UINT_MAX
			Record(std::vector<Interface> &interfaces, Interface *from_interface,
					uint from_interface_metrics) {
				for (auto &interface : interfaces) {
					if (&interface == from_interface) {
						data[&interface] = from_interface_metrics;
					} else {
						data[&interface] = UINT_MAX;
					}
				}
				this->min_metrics = from_interface_metrics;
				this->goto_interface = from_interface;
			}

			// Used unordered_map even though #elements == active_interfaces.size()
			// due to O(1) search used in MergeRoutingTables method
			std::unordered_map<Interface *, uint> data;

			// Precomputed minimum metrics and corresponding interface
			uint min_metrics;
			Interface *goto_interface;
		};

		struct SimpleAddressHash {
			typedef simulation::SimpleAddress argument_type;
			typedef size_t result_type;
			result_type operator()(argument_type const &simple_address) {
				return std::hash<uint32_t>()(simple_address.get_address());
			}
		};

		// Reference to the active interfaces on given node needed to create new
		// records
		const std::vector<Interface> &active_interfaces_;

	  std::unordered_map<SimpleAddress, Record, SimpleAddressHash> table_;
	};

	RoutingTable routing_table_;
};

}  // namespace simulation

#endif  // SARP_SRC_SIMULATION_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
