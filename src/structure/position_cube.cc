//
// position_cube.cc
//

#include <algorithm>
#include <cassert>
#include <sstream>

#include "structure/position.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const PositionCube &position_cube) {
  return os << '|' << position_cube.x << ',' << position_cube.y << ','
            << position_cube.z << '|';
}

PositionCube::PositionCube(uint32_t x, uint32_t y, uint32_t z)
    : x(x), y(y), z(z) {}

PositionCube::PositionCube(const Position &p, uint32_t connection_range) {
  uint32_t min_cube_side = connection_range;
  assert(min_cube_side != 0);
  x = p.x / min_cube_side;
  y = p.y / min_cube_side;
  z = p.z / min_cube_side;
}

std::pair<PositionCube, bool> PositionCube::GetRelativeCube(
    const int relative_pos[3]) const {
  int signed_x = x;
  int signed_y = y;
  int signed_z = z;

  signed_x += relative_pos[0];
  signed_y += relative_pos[1];
  signed_z += relative_pos[2];

  if (signed_x < 0 || signed_y < 0 || signed_z < 0) {
    return std::make_pair(PositionCube(), false);
  }
  return std::make_pair(PositionCube(signed_x, signed_y, signed_z), true);
}

int PositionCube::Distance(const PositionCube &pos1, const PositionCube &pos2) {
  uint32_t dx = std::abs((int)pos1.x - (int)pos2.x);
  uint32_t dy = std::abs((int)pos1.y - (int)pos2.y);
  uint32_t dz = std::abs((int)pos1.z - (int)pos2.z);
  return std::max(std::max(dx, dy), dz);
}

static std::size_t get_max_cube_side(const Position &min, const Position &max) {
  uint32_t dx = std::abs(max.x - min.x);
  uint32_t dy = std::abs(max.y - min.y);
  uint32_t dz = std::abs(max.z - min.z);
  return std::max(dx, std::max(dy, dz));
}

std::size_t PositionCube::GetID(Position min_pos, Position max_pos,
                                uint32_t connection_range) const {
  int cube_side = get_max_cube_side(min_pos, max_pos);
  int min_cube_side = connection_range;

  // WARNING: to keep GetID a 1-universal function for all possible node
  // positions we have to add +2 to max index instead of 1 since some
  // neighboring nodes will coordinate == cube_side / min_cube_side + 1.
  int max_index = cube_side / min_cube_side + 2;
  return x + y * max_index + z * max_index * max_index;
}

bool PositionCube::operator==(const PositionCube &other) const {
  return x == other.x && y == other.y && z == other.z;
}

bool PositionCube::operator!=(const PositionCube &other) const {
  return !(*this == other);
}

}  // namespace simulation
