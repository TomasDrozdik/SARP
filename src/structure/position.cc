//
// position.cc
//

#include "structure/position.h"

#include <cmath>
#include <sstream>

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Position &pos) {
  return os << '[' << pos.x << ',' << pos.y << ',' << pos.z << ']';
}

Position::Position(unsigned x, unsigned y, unsigned z)
    : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)) {}

double Position::Distance(const Position &pos1, const Position &pos2) {
  int dx = pos1.x - pos2.x;
  int dy = pos1.y - pos2.y;
  int dz = pos1.z - pos2.z;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool Position::operator==(const Position &other) const {
  return x == other.x && y == other.y && z == other.z;
}

}  // namespace simulation
