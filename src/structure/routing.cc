//
// routing.cc
//

#include "routing.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Routing &r) {
  return os << "R" << r.node_;
}

Routing::Routing(Node &node) : node_(node) { }

Routing::~Routing() { }

}  // namespace simulation
