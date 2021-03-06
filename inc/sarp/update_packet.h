//
// update_packet.h
//

#ifndef SARP_SARP_UPDATE_PACKET_H_
#define SARP_SARP_UPDATE_PACKET_H_

#include <cstdint>
#include <memory>

#include "sarp/routing.h"
#include "structure/packet.h"

namespace simulation {

class SarpUpdatePacket final : public Packet {
 public:
  SarpUpdatePacket(Address sender_address, Address destination_address,
                   SarpUpdate update)
      : Packet(sender_address, destination_address, PacketType::ROUTING,
               update.size() * sizeof(Cost)),
        update_(update) {}

  ~SarpUpdatePacket() override = default;

  SarpUpdate &&RetrieveUpdate() { return std::move(update_); }

 private:
  SarpUpdate update_;
};

}  // namespace simulation

#endif  // SARP_SARP_UPDATE_PACKET_H_
