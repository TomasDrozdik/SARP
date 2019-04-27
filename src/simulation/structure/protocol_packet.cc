//
// protocol_packet.cc
//

#include "protocol_packet.h"

namespace simulation {

ProtocolPacket::ProtocolPacket(std::unique_ptr<Address> sender_address,
		std::unique_ptr<Address> destination_address, uint32_t size_B) :
        sender_address_(std::move(sender_address)),
        destination_address_(std::move(destination_address)),
        size_B_(size_B) { }

const Address& ProtocolPacket::get_destination_address() {
  return *destination_address_;
}

const std::size_t ProtocolPacket::get_size() {
  return size_B_;
}

}  // namespace simulation
