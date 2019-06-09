//
// protocol_packet.cc
//

#include "protocol_packet.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const ProtocolPacket &packet) {
  return packet.Print(os);
}

ProtocolPacket::ProtocolPacket(std::unique_ptr<Address> sender_address,
    std::unique_ptr<Address> destination_address) :
        sender_address_(std::move(sender_address)),
        destination_address_(std::move(destination_address)),
        id_(++ProtocolPacket::id_base) { }

void ProtocolPacket::Process(Node &) { }

std::ostream &ProtocolPacket::Print(std::ostream &os) const {
  return os << '{' << id_ << '}';
}

const std::unique_ptr<Address>& ProtocolPacket::get_destination_address()
    const {
  return destination_address_;
}

std::size_t ProtocolPacket::get_size() const {
  return 0;
}

uint32_t ProtocolPacket::get_ttl() {
  return ttl_;
}

uint32_t ProtocolPacket::inc_ttl() {
  return ++ttl_;
}

}  // namespace simulation
