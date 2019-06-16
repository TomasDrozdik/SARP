//
// position.h
//

#ifndef SARP_STRUCTURE_POSITION_H_
#define SARP_STRUCTURE_POSITION_H_

#include <iostream>
#include <string>
#include <cstdint>

namespace simulation {

struct Position {
 friend std::ostream &operator<<(std::ostream &os, const Position &position);
 public:
  static uint32_t Distance(const Position &pos1, const Position &pos2);

  bool operator==(const Position &other) const;

  Position(int x = 0, int y = 0, int z = 0);

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
