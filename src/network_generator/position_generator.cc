//
// position_generator.cc
//

#include "position_generator.h"

#include <cstdlib>

int get_max(int from, int to) {
	return (from < to) ? std::rand() % (to - from) + from : 0;
}

namespace simulation {

FinitePositionGenerator::FinitePositionGenerator(
		const std::vector<Position> positions) : positions_(positions) { }

Position FinitePositionGenerator::operator++() {
	return (i < positions_.size()) ? positions_[i++] : positions_[(i = 0)];
}

RandomPositionGenerator::RandomPositionGenerator(
		int min_x, int max_x, int min_y, int max_y, int min_z, int max_z) :
				max_x_(max_x), max_y_(max_y), max_z_(max_z),
 				min_x_(min_x), min_y_(min_y), min_z_(min_z) { }

Position RandomPositionGenerator::operator++() {
	return Position(get_max(min_x_, max_x_), get_max(min_y_, max_y_),
			get_max(min_z_, max_z_));
}

}  // namespace simulation
