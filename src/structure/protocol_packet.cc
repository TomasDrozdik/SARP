//
// protocol_packet.cc
//

#include "structure/protocol_packet.h"

#include "structure/simulation.h"
#include "structure/simulation_parameters.h"
#include "structure/statistics.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const ProtocolPacket &packet) {
  return packet.Print(os);
}

ProtocolPacket::ProtocolPacket(Address sender_address,
                               Address destination_address,
                               PacketType packet_type, uint32_t size)
    : sender_address_(sender_address),
      destination_address_(destination_address),
      packet_type_(packet_type),
      size_(size),
      id_(ProtocolPacket::id_counter_++) {}

bool ProtocolPacket::IsTTLExpired() {
  if (++ttl_ == SimulationParameters::get_ttl_limit()) {
    Statistics::RegisterTTLExpire();
    return true;
  }
  return false;
}

std::ostream &ProtocolPacket::Print(std::ostream &os) const {
  return os << '{' << id_ << '}';
}

}  // namespace simulation
