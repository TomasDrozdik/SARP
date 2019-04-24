//
// interface.h
//

#ifndef SARP_SIMULATION_STRUCTURE_INTERFACE_H_
#define SARP_SIMULATION_STRUCTURE_INTERFACE_H_

#include <memory>
#include <vector>

#include "address.h"
#include "node.h"
#include "protocol_packet.h"
#include "routing.h"

namespace simulation {

class Node;

class Interface {
 public:
  Interface(Node *node, Node *other_end);

  void Send(std::unique_ptr<ProtocolPacket> packet) const;

  const std::vector<std::unique_ptr<Address>>& get_other_end_addresses() const;
  const Node* get_other_end() const;

 private:
  Node *node_;
  Node *other_end_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_INTERFACE_H_