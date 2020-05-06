//
// update_packet.cc
//

#include "sarp/cost.h"

#include "sarp/update_packet.h"

namespace simulation {

bool operator==(const Cost &c1, const Cost &c2) {
  return c1.mean == c2.mean && c1.sd == c2.sd;
}

bool operator!=(const Cost &c1, const Cost &c2) { return !(c1 == c2); }

}  // namespace simulation
