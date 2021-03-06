//
// position_generator.cc
//

#include "network_generator/position_generator.h"

#include <cassert>
#include <cstdlib>
#include <regex>
#include <string>

namespace simulation {

FinitePositionGenerator::FinitePositionGenerator(
    const std::vector<Position> &positions)
    : positions_(positions) {}

FinitePositionGenerator::FinitePositionGenerator(std::ifstream &is) {
  std::regex r(
      R"(\s*.+\s*\[\s*pos\s*=\s*"([[:digit:]]+)\s*,\s*([[:digit:]]+)\s*,\s*([[:digit:]]+)\s*!?"\s*\])");
  std::string line;
  while (std::getline(is, line)) {
    std::smatch match;
    if (std::regex_match(line, match, r)) {
      positions_.emplace_back(std::atoi(match[1].str().c_str()),
                              std::atoi(match[2].str().c_str()),
                              std::atoi(match[3].str().c_str()));
    }
  }
}

std::pair<Position, bool> FinitePositionGenerator::Next() {
  if (i < positions_.size()) {
    return std::make_pair(positions_[i++], true);
  }
  // Indicate generation end.
  return std::make_pair(Position(0, 0, 0), false);
}

std::unique_ptr<PositionGenerator> FinitePositionGenerator::Clone() {
  return std::make_unique<FinitePositionGenerator>(positions_);
}

RandomPositionGenerator::RandomPositionGenerator(range<Position> boundaries)
    : boundaries_(boundaries) {}

static int get_rand(int from, int to) {
  assert(from <= to);
  if (from == to) {
    return from;
  } else {
    return std::rand() % (to - from) + from;
  }
}

std::pair<Position, bool> RandomPositionGenerator::Next() {
  auto &[min, max] = boundaries_;
  return std::make_pair(Position(get_rand(min.x, max.x), get_rand(min.y, max.y),
                                 get_rand(min.z, max.z)),
                        true);
}

std::unique_ptr<PositionGenerator> RandomPositionGenerator::Clone() {
  return std::make_unique<RandomPositionGenerator>(boundaries_);
}

}  // namespace simulation
