//
// routing_update.cc
//

#include "distance_vector/update_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(
    Address sender_address, Address destination_address, const std::size_t &id,
    const DistanceVectorRouting::UpdateTable &update)
    : Packet(sender_address, destination_address, PacketType::ROUTING,
             1),  // size 1
      update(update),
      original_mirror_id_(id),
      mirror_id_(id) {
  size_ = 1;  // TODO: calculate properly
}

}  // namespace simulation
