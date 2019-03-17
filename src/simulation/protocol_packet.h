//
// protocol_packet.h
//

#ifndef SARP_SIMUALTION_PROTOCOL_PACKET_H_
#define SARP_SIMUALTION_PROTOCOL_PACKET_H_

namespace simulation {

class ProtocolPacket {
 public:
  virtual const Address& get_destination_address() = 0;

 private:
  const Address &destination_address;
};

}  // namespace simulation

#endif  // SARP_SIMUALTION_PROTOCOL_PACKET_H_