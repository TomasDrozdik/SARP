//
// update_packet.cc
//

#include "sarp/update_packet.h"

namespace simulation {

SarpUpdatePacket::SarpUpdatePacket(Address sender_address,
                                   Address destination_address,
                                   const std::size_t &id,
                                   const SarpRouting::UpdateTable &update)
    : Packet(sender_address, destination_address, PacketType::ROUTING,
             update.size() * sizeof(SarpRouting::Cost)),
      update(update),
      original_mirror_id_(id),
      mirror_id_(id) {}

}  // namespace simulation
