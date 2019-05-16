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
class ProtocolPacket;

class Interface {
 public:
  // Connects two nodes via interface
  // Both should be in Connection with each other via Connection.IsConnectedTo()
  // Interfaces shoul be added to both nodes with pointer to each other.
  static void Create(Node &node1, Node &node2);

  // Sends protocol packet over this interface
  void Send(std::unique_ptr<ProtocolPacket> packet) const;

  // Calls Recv on node with this interface as the processing interface
  void Recv(std::unique_ptr<ProtocolPacket> packet);

  // Debug prints, prints underlaying node .Print()
  void Print();

  const Node &get_other_end() const;
 private:
  Interface(Node &node);

  // Reference to a Node having this Interface
  Node &node_;

  // Has to be pointer due to cyclic nature of Interface.
  // Can't initialize it in ctor.
  const Interface *other_end_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_INTERFACE_H_
