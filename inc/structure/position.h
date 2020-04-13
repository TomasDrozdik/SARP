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

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
