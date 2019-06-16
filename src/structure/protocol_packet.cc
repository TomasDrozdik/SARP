//
// protocol_packet.cc
//

#include "protocol_packet.h"

#include "simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const ProtocolPacket &packet) {
  return packet.Print(os);
}

ProtocolPacket::ProtocolPacket(std::unique_ptr<Address> sender_address,
    std::unique_ptr<Address> destination_address, bool is_routing_update) :
        sender_address_(std::move(sender_address)),
        destination_address_(std::move(destination_address)),
        is_routing_update_(is_routing_update),
        id_(++ProtocolPacket::id_base) { }

bool ProtocolPacket::IsRoutingUpdate() const {
  return is_routing_update_;
}

bool ProtocolPacket::UpdateTTL() {
  if (++ttl_ ==
      Simulation::get_instance().get_simulation_parameters().get_ttl_limit()) {
    Simulation::get_instance().get_statistics().RegisterTTLExpire();
    return true;
  }
  return false;
}

std::ostream &ProtocolPacket::Print(std::ostream &os) const {
  return os << '{' << id_ << '}';
}

const std::unique_ptr<Address>& ProtocolPacket::get_sender_address()
    const {
  return sender_address_;
}

const std::unique_ptr<Address>& ProtocolPacket::get_destination_address()
    const {
  return destination_address_;
}

std::size_t ProtocolPacket::get_size() const {
  return 0;
}

uint32_t ProtocolPacket::get_ttl() const {
  return ttl_;
}

}  // namespace simulation
