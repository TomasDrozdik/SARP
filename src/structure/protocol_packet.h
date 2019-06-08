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
      std::unique_ptr<Address> destination_address);

  // The main function regarding the funcitonality of processing packets on
  // nodes. Nodes on Recv first call this function so it may change the contents
  // of the packet as well as do modifications on the given node. Therefore this
  // function carries a lot of weight and should be designed carefuly.
  virtual void Process(Node &node);
  virtual std::ostream &Print(std::ostream &os) const;

  virtual const Address& get_destination_address() const;
  virtual std::size_t get_size() const;

  // Functions to manipulate TTL
  uint32_t get_ttl();
  uint32_t inc_ttl();

 protected:
  std::unique_ptr<Address> sender_address_;
  std::unique_ptr<Address> destination_address_;

 private:
  static inline std::size_t id_base = 0;
  const std::size_t id_;
  uint32_t ttl_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_PROTOCOL_PACKET_H_
