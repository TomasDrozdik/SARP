//
// update_packet.h
//

#ifndef SARP_SARP_UPDATE_PACKET_H_
#define SARP_SARP_UPDATE_PACKET_H_

#include <cstdint>
#include <memory>

#include "sarp/routing.h"
#include "structure/protocol_packet.h"

namespace simulation {

class SarpRoutingUpdate final : public ProtocolPacket {
 public:
  SarpRoutingUpdate(std::unique_ptr<Address> sender_address,
                    std::unique_ptr<Address> destination_address,
                    SarpRoutingTable &&agregated_table);
  ~SarpRoutingUpdate() override = default;

  SarpRoutingTable update;
};

}  // namespace simulation

#endif  // SARP_SARP_UPDATE_PACKET_H_
