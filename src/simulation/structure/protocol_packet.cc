//
// protocol_packet.cc
//

#include "protocol_packet.h"

namespace simulation {

ProtocolPacket::ProtocolPacket(std::unique_ptr<Address> sender_address,
		std::unique_ptr<Address> destination_address) :
        sender_address_(std::move(sender_address)),
        destination_address_(std::move(destination_address)) { }

void ProtocolPacket::Process(Node &node) { }

const Address& ProtocolPacket::get_destination_address() const {
  return *destination_address_;
}

const std::size_t ProtocolPacket::get_size() const { return 0; }

}  // namespace simulation
