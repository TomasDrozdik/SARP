//
// static_routing.cc
//
#include <iostream>

#include "static_routing.h"

namespace simulation {

StaticRouting::StaticRouting(Node& node) : Routing(node) { }

Interface * StaticRouting::Route(ProtocolPacket &packet) {
  // Check for cycle
  if (packet.inc_ttl() ==
      Simulation::get_instance().get_simulation_parameters().get_ttl_limit()) {
    Simulation::get_instance().get_statistics().RegisterDetectedCycle();
    return nullptr;
  }
  auto search = mapping_.find(packet.get_destination_address());
  if (search == mapping_.end()) {
    return nullptr;
  }
  return search->second;
}

void StaticRouting::Init() { }

bool StaticRouting::AddRoute(const Node &to_node, const Node &via_node) {
  for (const auto& iface : node_.get_active_connections()) {
    if (&iface->get_other_end_node() == &via_node) {
      mapping_[to_node.get_address()->Clone()] =
          const_cast<Interface*>(iface.get());

      std::cerr << "STATIC ROUTING: connection node[" << *node_.get_address() <<
          "] to node[" << *to_node.get_address() << "]" << std::endl;

      return true;
    }
  }
  std::cerr << "STATIC ROUTING: connection unsuccessful!" << std::endl;
  return false;
}

}  // namespace simulation
