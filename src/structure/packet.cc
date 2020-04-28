//
// protocol_packet.cc
//

#include "structure/packet.h"

#include "structure/simulation.h"
#include "structure/statistics.h"

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

bool Packet::IsTTLExpired(uint32_t ttl_limit) {
  return ++ttl_ == ttl_limit;
}

std::ostream &Packet::Print(std::ostream &os) const {
  return os << '{' << id_ << '}';
}

}  // namespace simulation
