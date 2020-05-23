//
// types.cc
//

#include "structure/types.h"

#include <iostream>

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Address &addr) {
  os << '[';

  char delim = 0;
  for (const auto &addr_component : addr) {
    os << delim << (unsigned)addr_component;
    delim = '.';
  }
  return os << ']';
}

std::ostream &operator<<(std::ostream &os, const RoutingType &r) {
  switch (r) {
    case RoutingType::DISTANCE_VECTOR:
      os << "DISTANCE_VECTOR";
      break;
    case RoutingType::SARP:
      os << "SARP";
      break;
    default:
      assert(false);
  }
  return os;
}

}  // namespace simulation
