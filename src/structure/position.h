//
// position.h
//

#ifndef SARP_STRUCTURE_POSITION_H_
#define SARP_STRUCTURE_POSITION_H_

#include <iostream>
#include <string>

namespace simulation {

struct Position {
 friend std::ostream &operator<<(std::ostream &os, const Position &position);
 public:
  static int Distance(const Position &pos1, const Position &pos2);

  Position(int x, int y, int z);

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
