//
// routing.cc
//
#include "static_routing/routing.h"

#include <iostream>

namespace simulation {

StaticRouting::StaticRouting(Node &node) : Routing(node) {}

Node *StaticRouting::Route(ProtocolPacket &packet) {
  auto search = mapping_.find(packet.get_destination_address());
  if (search == mapping_.end()) {
    return nullptr;
  }
  return search->second;  // Return corresponding node.
}

bool StaticRouting::AddRoute(const Node &to_node, const Node &via_node) {
  auto it = node_.get_neighbors().find(const_cast<Node *>(&via_node));
  if (it == node_.get_neighbors().end()) {
    std::cerr << "STATIC ROUTING: connection unsuccessful! - NOT NEIGHBOR\n";
    return false;
  }
  // Using const_cast to work with effectively const value.
  mapping_[to_node.get_address()] = const_cast<Node *>(&via_node);
  std::cerr << "STATIC ROUTING: connection node[" << node_.get_address()
            << "] to node[" << to_node.get_address() << "]" << std::endl;
  return true;
}

}  // namespace simulation
