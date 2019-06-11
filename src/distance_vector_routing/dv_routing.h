//
// dv_routing.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
#define SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "../structure/address.h"
#include "../structure/interface.h"
#include "../structure/protocol_packet.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class Routing;

class DistanceVectorRouting final : public Routing {
 public:
	DistanceVectorRouting(Node &node);
	~DistanceVectorRouting() override = default;

	Interface *Route(ProtocolPacket &packet) override;

	bool Process(ProtocolPacket &packet,
			Interface *processing_interface) override;

	// Initializes the routing table with the set of interfaces on a given node
	// All the active interafaces would be at a 1 hop distance.
	void Init() override;

	// Sends table_ data to all direct neighbors.
  void Update() override;

 private:
	struct Record {
			// Creates new Record, fills all interfaces in data and initializes yet
			// unknow with max value
			Record(std::vector<std::unique_ptr<Interface>> &interfaces,
					Interface *from_interface, uint32_t from_interface_metrics);

			// Used unordered_map even though #elements == active_interfaces.size()
			// due to O(1) search used in MergeRoutingTables method
			std::unordered_map<Interface *, uint32_t> data_;
			// Precomputed minimum metrics and corresponding interface
			uint32_t min_metrics;
			Interface *goto_interface;
	};

	// Updates this with information form other RoutingTable incomming from
	// processing interface
	// RETURNS: true if change has occured, false otherwise
	bool UpdateRouting(const DistanceVectorRouting& other,
			Interface *processing_interface);

	std::unordered_map<std::unique_ptr<Address>, Record,
			AddressHash, AddressComparer> table_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_DV_ROUTING_H_
