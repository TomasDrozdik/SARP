//
// static_routing.cc
//
#include <iostream>

#include "static_routing.h"

namespace simulation {

StaticRouting::StaticRouting(Node& node) :
    Routing(node) { }

Interface const * const StaticRouting::Route(const Address &addr) const {
  auto search = mapping_.find(dynamic_cast<const SimpleAddress&>(addr));
  if (search == mapping_.end()) {
    return nullptr;
  }
  return search->second;
}

bool StaticRouting::ConnectToNode(const Node &node) {
  for (auto iface : node_.get_active_connections()) {
    if (iface.get_other_end() == &node) {
      SimpleAddress simple_addr(dynamic_cast<const SimpleAddress &>(
          *iface.get_other_end_addresses()[0]));
      mapping_[simple_addr] = &iface;

      std::cerr << "ROUTING: connection node[" <<
          dynamic_cast<const SimpleAddress &>(*node_.get_addresses()[0]).get_address() <<
          "] to node[" << simple_addr.get_address() << "]" << std::endl;

      return true;
    }
  }
  return false;
}

}  // namespace simulation
