//
// position.h
//

#ifndef SARP_SIMULATION_STRUCTURE_POSITION_H_
#define SARP_SIMULATION_STRUCTURE_POSITION_H_

#include <string>

namespace simulation {

struct Position {
 public:
  static int Distance(const Position &pos1, const Position &pos2);

  Position(int x, int y, int z);

  operator std::string() const;

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_POSITION_H_
