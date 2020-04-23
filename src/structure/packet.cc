//
// protocol_packet.cc
//

#include "structure/packet.h"

#include "structure/simulation.h"
#include "structure/simulation_parameters.h"
#include "structure/statistics.h"

extern std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Packet &packet) {
  return packet.Print(os);
}

Packet::Packet(Address sender_address, Address destination_address,
               PacketType packet_type, uint32_t size)
    : sender_address_(sender_address),
      destination_address_(destination_address),
      packet_type_(packet_type),
      size_(size),
      id_(Packet::id_counter_++) {}

bool Packet::IsTTLExpired() {
  if (++ttl_ == config->ttl_limit) {
    Statistics::RegisterTTLExpire();
    return true;
  }
  return false;
}

std::ostream &Packet::Print(std::ostream &os) const {
  return os << '{' << id_ << '}';
}

}  // namespace simulation
