//
// update_packet.h
//

#ifndef SARP_SARP_UPDATE_PACKET_H_
#define SARP_SARP_UPDATE_PACKET_H_

#include <memory>
#include <cstdint>

#include "routing.h"
#include "../structure/protocol_packet.h"

namespace simulation {

class SarpRoutingUpdate final : public ProtocolPacket {
 public:
  SarpRoutingUpdate(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address,
      SarpRoutingTable &table);
  ~SarpRoutingUpdate() override = default;

  std::size_t get_size() const override;

  SarpRoutingTable update;
};

}  // namespace simulation

#endif  // SARP_SARP_UPDATE_PACKET_H_