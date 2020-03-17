//
// position.h
//

#ifndef SARP_STRUCTURE_POSITION_H_
#define SARP_STRUCTURE_POSITION_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace simulation {

struct Position {
  friend std::ostream &operator<<(std::ostream &os, const Position &position);

 public:
  Position(int x = 0, int y = 0, int z = 0);

  static double Distance(const Position &pos1, const Position &pos2);

  bool operator==(const Position &other) const;

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
