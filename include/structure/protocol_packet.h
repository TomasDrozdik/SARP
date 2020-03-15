//
// protocol_packet.h
//

#ifndef SARP_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_STRUCTURE_PROTOCOL_PACKET_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "structure/address.h"
#include "structure/node.h"

namespace simulation {

class Node;

class ProtocolPacket {
  friend std::ostream &operator<<(std::ostream &os,
                                  const ProtocolPacket &packet);

 public:
  ProtocolPacket(std::unique_ptr<Address> sender_address,
                 std::unique_ptr<Address> destination_address,
                 bool is_routing_packet = false);
  ProtocolPacket(std::unique_ptr<Address> sender_address,
                 std::unique_ptr<Address> destination_address, uint32_t size);
  virtual ~ProtocolPacket() = default;

  virtual std::ostream &Print(std::ostream &os) const;

  // Increases TTL
  // RETURNS: true if TTL limit is reached, false otherwise.
  bool UpdateTTL();

  bool is_routing_update() const { return is_routing_update_; }

  const std::unique_ptr<Address> &get_sender_address() const {
    return sender_address_;
  }

  const std::unique_ptr<Address> &get_destination_address() const {
    return destination_address_;
  }

  uint32_t get_size() const { return size_; }

  uint32_t get_ttl() const { return ttl_; }

 protected:
  std::unique_ptr<Address> sender_address_;
  std::unique_ptr<Address> destination_address_;
  uint32_t size_ = 0;  // effectively cosnt
  bool is_routing_update_;

 private:
  static inline std::size_t id_counter_ = 0;

  const std::size_t id_;
  uint32_t ttl_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_PROTOCOL_PACKET_H_
