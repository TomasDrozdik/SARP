//
// routing_update.cc
//

#include "distance_vector/update_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(
    Address sender_address, Address destination_address, const std::size_t &id,
    const DistanceVectorRouting::RoutingTableType &table)
    : ProtocolPacket(sender_address, destination_address, PacketType::ROUTING,
                     1),  // size 1
      original_mirror_id(id),
      mirror_id(id),
      mirror_table(table) {
  size_ = mirror_table.size() * sizeof(DistanceVectorRouting::Record);
}

}  // namespace simulation
