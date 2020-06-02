//
// position_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_

#include <fstream>
#include <memory>
#include <vector>

#include "structure/position.h"
#include "structure/types.h"

namespace simulation {

class PositionGenerator {
 public:
  virtual ~PositionGenerator() = default;
  virtual std::pair<Position, bool> Next() = 0;
  virtual std::unique_ptr<PositionGenerator> Clone() = 0;
};

class FinitePositionGenerator final : public PositionGenerator {
 public:
  FinitePositionGenerator(const std::vector<Position> &positions);
  FinitePositionGenerator(std::ifstream &is);
  ~FinitePositionGenerator() override = default;

  std::pair<Position, bool> Next() override;

  std::unique_ptr<PositionGenerator> Clone() override;

 private:
  std::size_t i = 0;
  std::vector<Position> positions_;
};

class RandomPositionGenerator : public PositionGenerator {
 public:
  RandomPositionGenerator(range<Position> boundaries);
  ~RandomPositionGenerator() override = default;

  std::pair<Position, bool> Next() override;

  std::unique_ptr<PositionGenerator> Clone() override;

 private:
  range<Position> boundaries_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_POSITION_GENERATOR_H_
