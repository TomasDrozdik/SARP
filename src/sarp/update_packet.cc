//
// update_packet.cc
//

#include "sarp/update_packet.h"

namespace simulation {

SarpUpdatePacket::SarpUpdatePacket(Address sender_address,
                                   Address destination_address,
                                   const std::size_t &id,
                                   const SarpRouting::UpdateTable &table)
    // TODO: Update size, currently 1.
    : Packet(sender_address, destination_address, PacketType::ROUTING,
             table.size() * sizeof(SarpRouting::CostInfo)),
      update_mirror(table),
      original_mirror_id_(id),
      mirror_id_(id) {}

}  // namespace simulation
