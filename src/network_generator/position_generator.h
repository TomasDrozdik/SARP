//
// position_generator.h
//

#ifndef SARP_SRC_SIMULATION_NETWORK_GENERATOR_POSITION_GENERATOR_H_
#define SARP_SRC_SIMULATION_NETWORK_GENERATOR_POSITION_GENERATOR_H_

#include "../structure/position.h"

namespace simulation {

class PositionGenerator {
 public:
	PositionGenerator(int min_x, int max_x, int min_y, int max_y,
			int min_z, int max_z);

	virtual Position operator++();

 private:
  int max_x_, max_y_, max_z_;
  int min_x_, min_y_, min_z_;
};


}  // namespace simulation

#endif  // SARP_SRC_SIMULATION_NETWORK_GENERATOR_POSITION_GENERATOR_H_