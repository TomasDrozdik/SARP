//
// position_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_

#include <vector>

#include "../structure/position.h"

namespace simulation {

class PositionGenerator {
 public:
  virtual ~PositionGenerator() = default;
	virtual Position operator++() = 0;
};

class FinitePositionGenerator : public PositionGenerator {
 public:
  FinitePositionGenerator(const std::vector<Position> positions);
	~FinitePositionGenerator() override = default;

	Position operator++() override;

 private:
  std::size_t i = 0;
	const std::vector<Position> positions_;
};

class RandomPositionGenerator : public PositionGenerator {
 public:
	RandomPositionGenerator(int min_x, int max_x, int min_y, int max_y,
			int min_z, int max_z);
	RandomPositionGenerator(int min, int max);
	~RandomPositionGenerator() override = default;

	Position operator++() override;

 private:
  const int max_x_, max_y_, max_z_;
  const int min_x_, min_y_, min_z_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_