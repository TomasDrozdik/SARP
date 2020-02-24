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
        id_(ProtocolPacket::id_counter_++) { }

ProtocolPacket::ProtocolPacket(std::unique_ptr<Address> sender_address,
    std::unique_ptr<Address> destination_address, uint32_t size) :
        ProtocolPacket(std::move(sender_address),
            std::move(destination_address)) {
  size_ = size;
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

}  // namespace simulation
