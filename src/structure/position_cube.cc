//
// position_cube.cc
//

#include "structure/position_cube.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "structure/simulation_parameters.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const PositionCube &position_cube) {
  return os << '|' << position_cube.x << ',' << position_cube.y << ','
            << position_cube.z << '|';
}

PositionCube::PositionCube(uint32_t x, uint32_t y, uint32_t z)
    : x(x), y(y), z(z) {}

PositionCube::PositionCube(const Position &p) {
  uint32_t min_cube_side = SimulationParameters::get_connection_range();
  assert(min_cube_side != 0);
  uint32_t min_cube_x_index = p.x / min_cube_side;
  uint32_t min_cube_y_index = p.y / min_cube_side;
  uint32_t min_cube_z_index = p.z / min_cube_side;
  x = min_cube_x_index;
  y = min_cube_y_index;
  z = min_cube_z_index;
}

bool PositionCube::GetRelativeCube(const int relative_pos[3],
                                   PositionCube *out) const {
  int signed_x = x;
  int signed_y = y;
  int signed_z = z;

  signed_x += relative_pos[0];
  signed_y += relative_pos[1];
  signed_z += relative_pos[2];

  if (signed_x < 0 || signed_y < 0 || signed_z < 0) {
    return false;
  }
  *out = PositionCube(signed_x, signed_y, signed_z);
  return true;
}

int PositionCube::Distance(const PositionCube &pos1, const PositionCube &pos2) {
  uint32_t dx = std::abs((int)pos1.x - (int)pos2.x);
  uint32_t dy = std::abs((int)pos1.y - (int)pos2.y);
  uint32_t dz = std::abs((int)pos1.z - (int)pos2.z);
  return std::min(std::min(dx, dy), dz);
}

std::size_t PositionCube::GetID() const {
  int cube_side = SimulationParameters::get_max_cube_side();
  int min_cube_side = SimulationParameters::get_connection_range();

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
