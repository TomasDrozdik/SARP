//
// address.cc
//

#include "structure/address.h"

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

}  // namespace simulation
