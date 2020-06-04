//
// update_packet.cc
//

#include "sarp/cost.h"

#include "sarp/update_packet.h"

namespace simulation {

bool operator==(const Cost &lhs, const Cost &rhs) {
  return lhs.mean_ == rhs.mean_ && lhs.variance_ == rhs.variance_;
}

bool operator!=(const Cost &lhs, const Cost &rhs) { return !(lhs == rhs); }

std::ostream &operator<<(std::ostream &os, const Cost &cost) {
  return os << "{.mean = " << cost.Mean() << " .variance = " << cost.Variance()
            << '}';
}

}  // namespace simulation
