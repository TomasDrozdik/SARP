//
// protocol_packet.h
//

#ifndef SARP_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_STRUCTURE_PROTOCOL_PACKET_H_

#include <iostream>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "address.h"
#include "node.h"

namespace simulation {

class Node;

class ProtocolPacket {
 friend std::ostream &operator<<(std::ostream &os,
    const ProtocolPacket &packet);
 public:
  ProtocolPacket(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address,
      bool is_routing_update = false);
  virtual ~ProtocolPacket() = default;

  virtual bool IsRoutingUpdate() const;
  virtual std::ostream &Print(std::ostream &os) const;
  virtual std::size_t get_size() const;

  // Increases TTL
  // RETURNS: true if TTL limit is reached, false otherwise.
  bool UpdateTTL();

  const std::unique_ptr<Address>& get_sender_address() const;
  const std::unique_ptr<Address>& get_destination_address() const;
  uint32_t get_ttl() const;

 protected:
  std::unique_ptr<Address> sender_address_;
  std::unique_ptr<Address> destination_address_;
  bool is_routing_update_;

 private:
  static inline std::size_t id_base = 0;
  const std::size_t id_;
  uint32_t ttl_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_PROTOCOL_PACKET_H_
