//
// position_cube.h
//

#ifndef SARP_STRUCTURE_POSITION_CUBE_H_
#define SARP_STRUCTURE_POSITION_CUBE_H_

#include <iostream>

#include "structure/position.h"

namespace simulation {

struct PositionCube final {
  friend std::ostream &operator<<(std::ostream &os,
                                  const PositionCube &position_cube);

 public:
  PositionCube(int x, int y, int z);

  PositionCube(const Position &p);

  static int Distance(const PositionCube &pos1, const PositionCube &pos2);

  std::size_t GetID() const;

  bool operator==(const PositionCube &other) const;

  bool operator!=(const PositionCube &other) const;

  PositionCube operator+(const PositionCube &other) const;

 private:
  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_CUBE_H_
