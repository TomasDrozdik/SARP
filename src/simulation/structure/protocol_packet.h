//
// protocol_packet.h
//

#ifndef SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_

namespace simulation {

class ProtocolPacket {
 public:
  virtual const Address& get_destination_address() = 0;
  virtual const uint get_size() = 0;

 private:
  const Address &destination_address;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_