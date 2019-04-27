//
// protocol_packet.h
//

#ifndef SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_

#include <memory>
#include <cstddef>

#include "address.h"

namespace simulation {

class ProtocolPacket {
 public:
  ProtocolPacket(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address, uint32_t size_B = 50);
  virtual const Address& get_destination_address();
  virtual const std::size_t get_size();

 private:
  std::unique_ptr<Address> sender_address_;
  std::unique_ptr<Address> destination_address_;
  uint32_t size_B_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
