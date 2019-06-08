//
// dv_routing.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
#define SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_

#include <functional>
#include <vector>
#include <unordered_map>
#include <climits>

#include "../structure/address.h"
#include "../structure/interface.h"
#include "../structure/protocol_packet.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace std {

	template<>
	struct hash<simulation::SimpleAddress> {
		size_t operator()(const simulation::SimpleAddress &simple_addr) const {
			return hash<uint32_t>()(simple_addr.get_address());
		}
	};

}  // namespace std

namespace simulation {

class Routing;

class DistanceVectorRouting final : public Routing {
 public:
	DistanceVectorRouting(Node &node);
	Interface * Route(ProtocolPacket &packet) override;
	void Init() override;
 private:
	class RoutingTable {
	 public:
		// Record in the routing table assigned to given address
		// Used instead of tuple for better readability and default initialization
		// with ctor
		struct Record {
			// Creates new Record fills all interfaces in data and initializes yet
			// unknow with UINT_MAX
			Record(std::vector<Interface> &interfaces,
					Interface *from_interface, uint from_interface_metrics);

			// Used unordered_map even though #elements == active_interfaces.size()
			// due to O(1) search used in MergeRoutingTables method
			std::unordered_map<const Interface *, uint> data_;
			// Precomputed minimum metrics and corresponding interface
			uint min_metrics;
			Interface *goto_interface;
		};

	  RoutingTable(std::vector<Interface> &active_interfaces);

	  // Initializes the routing table with the set of interfaces on a given node
		// All the active interafaces would be at a 1 hop distance
		void Init();

		// Finds metrics defined shortest route to to_address
		Interface * Route(ProtocolPacket &to_address) const;

		// Updates this with information form other RoutingTable incomming from
		// processing interface
		// RETURNS: true if change has occured, false otherwise
		bool MergeRoutingTables(const RoutingTable& other,
				Interface *processing_interface);

	 private:
		std::vector<Interface> &active_interfaces_;
	  std::unordered_map<SimpleAddress, Record> table_;
	};

	RoutingTable routing_table_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
