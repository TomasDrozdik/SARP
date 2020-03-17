//
// position_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_

#include <fstream>
#include <vector>

#include "structure/position.h"

namespace simulation {

class PositionGenerator {
 public:
  virtual ~PositionGenerator() = default;
  virtual Position operator++() = 0;
};

class FinitePositionGenerator : public PositionGenerator {
 public:
  FinitePositionGenerator(const std::vector<Position> &positions);
  FinitePositionGenerator(std::ifstream &is);
  ~FinitePositionGenerator() override = default;

  Position operator++() override;

 private:
  std::size_t i = 0;
  std::vector<Position> positions_;
};

class RandomPositionGenerator : public PositionGenerator {
 public:
  RandomPositionGenerator(Position min, Position max);
  ~RandomPositionGenerator() override = default;

  Position operator++() override;

 private:
  Position min_;
  Position max_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_
