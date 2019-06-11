//
// dv_packet.cc
//

#include "dv_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(std::unique_ptr<Address> sender_address,
		std::unique_ptr<Address> destination_address,
		const DistanceVectorRouting &routing_update) :
				ProtocolPacket(std::move(sender_address),
						std::move(destination_address), true),
				routing_update(routing_update) { }

std::size_t DVRoutingUpdate::get_size() const {
	// TODO count properly
	return 100;  // To count routing packets.
}

}  // namcespace simulation
