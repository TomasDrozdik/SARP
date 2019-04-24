//
// protocol_packet.cc
//

#include "protocol_packet.h"

namespace simulation {

ProtocolPacket::ProtocolPacket(const Address &destination_address) :
    destination_address_(destination_address) { }


const Address& ProtocolPacket::get_destination_address() {
  return destination_address_;
}

const std::size_t ProtocolPacket::get_size() {
  return sizeof (this);
}

}  // namespace simulation
