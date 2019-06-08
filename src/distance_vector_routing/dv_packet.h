//
// dv_packet.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_
#define SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_

#include <memory>
#include <cstdint>

#include "../structure/protocol_packet.h"

namespace simulation {

class DVRoutingUpdate final : public ProtocolPacket {
 public:
  DVRoutingUpdate(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address);
  void Process(Node &node) override;
  const Address& get_destination_address() const override;
  std::size_t get_size() const override;

 private:
  uint hop_count_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_
