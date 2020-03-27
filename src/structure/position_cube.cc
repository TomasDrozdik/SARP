//
// position_cube.cc
//

#include "structure/position_cube.h"

#include <algorithm>
#include <sstream>

#include "structure/simulation_parameters.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const PositionCube &position_cube) {
  return os << '|' << position_cube.x << ',' << position_cube.y << ','
            << position_cube.z << '|';
}

PositionCube::PositionCube(int x, int y, int z) : x(x), y(y), z(z) {}

PositionCube::PositionCube(const Position &p) {
  uint32_t min_cube_side = SimulationParameters::get_connection_range();
  uint32_t min_cube_x_index = p.x / min_cube_side;
  uint32_t min_cube_y_index = p.y / min_cube_side;
  uint32_t min_cube_z_index = p.z / min_cube_side;
  x = min_cube_x_index;
  y = min_cube_y_index;
  z = min_cube_z_index;
}

int PositionCube::Distance(const PositionCube &pos1, const PositionCube &pos2) {
  int dx = std::abs(pos1.x - pos2.x);
  int dy = std::abs(pos1.y - pos2.y);
  int dz = std::abs(pos1.z - pos2.z);
  return std::min(std::min(dx, dy), dz);
}

std::size_t PositionCube::GetID() const {
  int cube_side = SimulationParameters::get_max_cube_side();
  int min_cube_side = SimulationParameters::get_connection_range();
  int max_index = cube_side / min_cube_side + 1;
  return x + y * max_index + z * max_index * max_index;
}

bool PositionCube::operator==(const PositionCube &other) const {
  return x == other.x && y == other.y && z == other.z;
}

bool PositionCube::operator!=(const PositionCube &other) const {
  return !(*this == other);
}

PositionCube PositionCube::operator+(const PositionCube &other) const {
  return PositionCube(x + other.x, y + other.y, z + other.z);
}

}  // namespace simulation
