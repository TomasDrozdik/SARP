//
// position.cc
//

#include "position.h"

#include <sstream>
#include <cmath>

namespace simulation {

Position::Position(int x, int y, int z) : x(x), y(y), z(z) { }

int Position::Distance(const Position &pos1, const Position &pos2) {
    int dx = pos1.x - pos2.x;
    int dy = pos1.y - pos2.y;
    int dz = pos1.z - pos2.z;
    return (std::sqrt(dx*dx + dy*dy + dz*dz));
}

// TODO: can be done more efficiently
Position::operator std::string() const {
  std::ostringstream oss;
  oss << "(" << x << "," << y << "," << z << ")";
  return oss.str();
}

}  // namespace simulation
