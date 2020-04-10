//
// update_packet.cc
//

#include "sarp/update_packet.h"

namespace simulation {

SarpUpdatePacket::SarpUpdatePacket(Address sender_address,
                                   Address destination_address,
                                   const std::size_t &id,
                                   const SarpRouting::RoutingTableType &table)
    : ProtocolPacket(sender_address, destination_address, PacketType::ROUTING,
                     1),  // size 1
      mirror_table(table),
      original_mirror_id_(id),
      mirror_id_(id) {
  size_ = mirror_table.size() * sizeof(SarpRouting::Record);
}

}  // namespace simulation
