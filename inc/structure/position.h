//
// position.h
//

#ifndef SARP_STRUCTURE_POSITION_H_
#define SARP_STRUCTURE_POSITION_H_

#include <iostream>

namespace simulation {

struct Position final {
  friend std::ostream &operator<<(std::ostream &os, const Position &position);

 public:
  // WARNING: do not input negative values because it may break neighbor
  // detection.
  Position(int x = 0, int y = 0, int z = 0);

  static double Distance(const Position &pos1, const Position &pos2);

  bool operator==(const Position &other) const;

  int x, y, z;
};

struct PositionCube final {
  friend std::ostream &operator<<(std::ostream &os,
                                  const PositionCube &position_cube);

 public:
  PositionCube() = default;
  PositionCube(uint32_t x, uint32_t y, uint32_t z);
  PositionCube(const Position &p, uint32_t connection_range);

  static int Distance(const PositionCube &pos1, const PositionCube &pos2);

  std::size_t GetID(Position min_pos, Position max_pos,
                    uint32_t connection_range) const;

  std::pair<PositionCube, bool> GetRelativeCube(
      const int relative_pos[3]) const;

  bool operator==(const PositionCube &other) const;

  bool operator!=(const PositionCube &other) const;

 private:
  uint32_t x, y, z;
};



}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
