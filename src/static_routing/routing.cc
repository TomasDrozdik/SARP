//
// routing.cc
//
#include "static_routing/routing.h"

#include <iostream>

namespace simulation {

void StaticRouting::Connect(Network &network, std::size_t on_node_idx,
                            std::size_t to_node_idx, std::size_t via_node_idx) {
  Node &node = network.get_nodes()[on_node_idx];
  Node &to_node = network.get_nodes()[to_node_idx];
  Node &via_node = network.get_nodes()[via_node_idx];
  dynamic_cast<StaticRouting &>(node.get_routing()).AddRoute(to_node, via_node);
}

StaticRouting::StaticRouting(Node &node) : Routing(node) {}

Node *StaticRouting::Route(Env &, Packet &packet) {
  auto search = mapping_.find(packet.get_destination_address());
  if (search == mapping_.end()) {
    return nullptr;
  }
  return search->second;  // Return corresponding node.
}

bool StaticRouting::AddRoute(const Node &to_node, const Node &via_node) {
  // Using const_cast to work with effectively const value.
  mapping_[to_node.get_address()] = const_cast<Node *>(&via_node);
  std::cerr << "STATIC ROUTING: connection node[" << node_.get_address()
            << "] to node[" << to_node.get_address() << "]" << std::endl;
  return true;
}

}  // namespace simulation
