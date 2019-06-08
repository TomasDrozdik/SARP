//
// position.cc
//

#include "position.h"

#include <sstream>
#include <cmath>

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Position &pos) {
  return os << "(" << pos.x << "," << pos.y << "," << pos.z << ")";
}

Position::Position(int x, int y, int z) : x(x), y(y), z(z) { }

int Position::Distance(const Position &pos1, const Position &pos2) {
    int dx = pos1.x - pos2.x;
    int dy = pos1.y - pos2.y;
    int dz = pos1.z - pos2.z;
    return (std::sqrt(dx*dx + dy*dy + dz*dz));
}

}  // namespace simulation
