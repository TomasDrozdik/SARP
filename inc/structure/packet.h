//
// packet.h
//

#ifndef SARP_STRUCTURE_PACKET_H_
#define SARP_STRUCTURE_PACKET_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "structure/address.h"
#include "structure/node.h"

namespace simulation {

class Node;

enum class PacketType {
  ROUTING,
  DATA,
};

class Packet {
  friend std::ostream &operator<<(std::ostream &os,
                                  const Packet &packet);

 public:
  Packet(Address sender_address, Address destination_address,
                 PacketType packet_type, uint32_t size);

  virtual ~Packet() = default;

  virtual std::ostream &Print(std::ostream &os) const;

  // Increases TTL
  // RETURNS: true if TTL limit is reached, false otherwise.
  bool IsTTLExpired();

  bool IsRoutingUpdate() const { return packet_type_ == PacketType::ROUTING; }

  const Address &get_sender_address() const { return sender_address_; }

  const Address &get_destination_address() const {
    return destination_address_;
  }

  uint32_t get_size() const { return size_; }

  uint32_t get_ttl() const { return ttl_; }

 protected:
  // Effectively const values
  const Address sender_address_;
  const Address destination_address_;
  const PacketType packet_type_;
  uint32_t size_;

 private:
  static inline std::size_t id_counter_ = 0;

  const std::size_t id_;
  uint32_t ttl_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_PACKET_H_
