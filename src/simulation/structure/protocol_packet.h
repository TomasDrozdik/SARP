//
// protocol_packet.h
//

#ifndef SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_

#include <cstddef>

#include "address.h"

namespace simulation {

class ProtocolPacket {
 public:
  ProtocolPacket(const Address &destination_address);
  virtual const Address& get_destination_address();
  virtual const std::size_t get_size();

 private:
  const Address &destination_address_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
