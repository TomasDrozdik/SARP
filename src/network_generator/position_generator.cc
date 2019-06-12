//
// position_generator.cc
//

#include "position_generator.h"

#include <string>
#include <regex>
#include <cstdlib>

int get_rand(int from, int to) {
	return (from < to) ? std::rand() % (to - from) + from : 0;
}

namespace simulation {

FinitePositionGenerator::FinitePositionGenerator(
		const std::vector<Position> &positions) : positions_(positions) { }

FinitePositionGenerator::FinitePositionGenerator(std::ifstream &is) {
	std::regex r(R"(\s*.+\s*\[\s*pos\s*=\s*"([[:digit:]]+)\s*,\s*([[:digit:]]+)\s*,\s*([[:digit:]]+)\s*!?"\s*\])");
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

Position FinitePositionGenerator::operator++() {
	return (i < positions_.size()) ? positions_[i++] : positions_[(i = 0)];
}

RandomPositionGenerator::RandomPositionGenerator(Position max, Position min) :
		max_(max), min_(min) { }

Position RandomPositionGenerator::operator++() {
	return Position(get_rand(min_.x, max_.x), get_rand(min_.y, max_.y),
			get_rand(min_.z, max_.z));
}

}  // namespace simulation
