//
// position.h
//

#ifndef SARP_SIMULATION_STRUCTURE_POSITION_H_
#define SARP_SIMULATION_STRUCTURE_POSITION_H_

namespace simulation {

struct Position {
 public:
  int x, y, z;

  static int Distance(const Position &pos1, const Position &pos2);
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_POSITION_H_
