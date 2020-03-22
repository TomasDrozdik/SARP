//
// routing.cc
//
#include "static_routing/routing.h"

#include <iostream>

namespace simulation {

StaticRouting::StaticRouting(Node &node) : Routing(node) {}

Interface *StaticRouting::Route(ProtocolPacket &packet) {
  auto search = mapping_.find(packet.get_destination_address());
  if (search == mapping_.end()) {
    return nullptr;
  }
  return search->second;  // Return corresponding interface.
}

bool StaticRouting::AddRoute(const Node &to_node, const Node &via_node) {
  // Find interface which matches via_node argument
  for (const auto &iface : node_.get_active_interfaces()) {
    if (&iface->get_other_end_node() == &via_node) {
      // Using const_cast to work with effectively const value.
      mapping_[to_node.get_address()] = const_cast<Interface *>(iface.get());

      std::cerr << "STATIC ROUTING: connection node[" << node_.get_address()
                << "] to node[" << to_node.get_address() << "]" << std::endl;

      return true;
    }
  }
  std::cerr << "STATIC ROUTING: connection unsuccessful!" << std::endl;
  return false;
}

}  // namespace simulation
