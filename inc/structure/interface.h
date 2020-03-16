//
// interface.h
//

#ifndef SARP_STRUCTURE_INTERFACE_H_
#define SARP_STRUCTURE_INTERFACE_H_

#include <iostream>
#include <memory>
#include <vector>

#include "structure/address.h"
#include "structure/node.h"
#include "structure/protocol_packet.h"
#include "structure/routing.h"

namespace simulation {

class Node;
class ProtocolPacket;

class Interface {
  friend std::ostream &operator<<(std::ostream &os, const Interface &iface);

  // Updating interfaces on Network is more efficient with direct private
  // modifications.
  friend class Network;

 public:
  // Marks other_end_ as invalid.
  ~Interface();

  // Connects two nodes via interface
  // Both should be in connection with each other via Connection.IsConnectedTo()
  // Interfaces should be added to both nodes with pointer to each other.
  static void Create(Node &node1, Node &node2);

  // This ctor is public only for make_unique. Use Create instead.
  Interface(Node &node, Node &other);

  // Sends protocol packet over this interface.
  void Send(std::unique_ptr<ProtocolPacket> packet) const;

  // Calls Recv on node with this interface as the processing interface.
  void Recv(std::unique_ptr<ProtocolPacket> packet);

  // Checks if interface nodes are connected and interface is valid.
  bool IsConnected() const;

  // Compares interafaces bitwise.
  bool operator==(const Interface &other) const;

  const Node &get_node() const;
  const Node &get_other_end_node() const;
  const Interface &get_other_end() const;

 private:
  // Pointers to a Node having this Interface.
  Node *node_;

  // Pointer to node on the other end i.e. other_end_->node
  // Although it may seem this is redundant I keep it here to be able to search
  // for matching interfaces i.e. node_ and other_node_ are the same.
  // See Network::UpdateInterfaces().
  Node *other_node_;

  // Has to be pointer due to cyclic nature of Interface.
  // Can't initialize it in ctor.
  Interface *other_end_;

  // Other interface other_end_ can mark this one invalid on it's termination.
  bool is_valid_ = true;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_INTERFACE_H_
