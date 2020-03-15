//
// position.h
//

#ifndef SARP_STRUCTURE_POSITION_H_
#define SARP_STRUCTURE_POSITION_H_

#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace simulation {

struct Position {
  friend std::ostream &operator<<(std::ostream &os, const Position &position);

 public:
  using Unit = uint32_t;

  static inline double Distance(const Position &pos1, const Position &pos2) {
    int dx = pos1.x - pos2.x;
    int dy = pos1.y - pos2.y;
    int dz = pos1.z - pos2.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }

  static inline std::unique_ptr<Position> max;
  static inline std::unique_ptr<Position> min;

  static void set_max(Position pos);
  static void set_min(Position pos);

  bool operator==(const Position &other) const;

  Position(int x = 0, int y = 0, int z = 0);

  int x, y, z;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_POSITION_H_
