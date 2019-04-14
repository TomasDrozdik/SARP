//
// position.h
//

#ifndef SARP_SIMULATION_STRUCTURE_POSITION_H_
#define SARP_SIMULATION_STRUCTURE_POSITION_H_

namespace simulation {

struct Position {
 public:
  static int Distance(const Position &pos1, const Position &pos2);

  Position(int x, int y, int z);

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_POSITION_H_
