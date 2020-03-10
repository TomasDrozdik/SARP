//
// position.cc
//

#include "structure/position.h"

#include <sstream>
#include <cmath>

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Position &pos) {
  return os << "pos = \"" << pos.x << "," << pos.y << "," << pos.z << "!\"";
}

Position::Position(int x, int y, int z) : x(x), y(y), z(z) { }

void Position::set_max(Position pos) {
  Position::max = std::make_unique<Position>(pos);
}

void Position::set_min(Position pos) {
  Position::min = std::make_unique<Position>(pos);
}

bool Position::operator==(const Position &other) const {
  return x == other.x && y == other.y && z == other.z;
}

}  // namespace simulation
