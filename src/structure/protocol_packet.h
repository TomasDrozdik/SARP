//
// protocol_packet.h
//

#ifndef SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
#define SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_

#include <memory>
#include <cstddef>

#include "address.h"
#include "node.h"

namespace simulation {

class Node;

class ProtocolPacket {
 public:
  ProtocolPacket(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address);

  // The main function regarding the funcitonality of processing packets on
  // nodes. Nodes on Recv first call this function so it may change the contents
  // of the packet as well as do modifications on the given node. Therefore this
  // function carries a lot of weight and should be designed carefuly.
  virtual void Process(Node &node);

  virtual const Address& get_destination_address() const;
  virtual std::size_t get_size() const;
 private:
  std::unique_ptr<Address> sender_address_;
  std::unique_ptr<Address> destination_address_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_PROTOCOL_PACKET_H_
