//
// time_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_TIME_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_TIME_GENERATOR_H_

#include <fstream>
#include <memory>
#include <vector>

namespace simulation {

using Time = std::size_t;

class TimeGenerator {
 public:
  virtual ~TimeGenerator() = default;

  virtual std::pair<Time, bool> Next() { return {0, true}; }

  virtual std::unique_ptr<TimeGenerator> Clone() {
    return std::make_unique<TimeGenerator>();
  }
};

class FiniteTimeGenerator final : public TimeGenerator {
 public:
  FiniteTimeGenerator(std::vector<Time> time) : times_(time) {}

  ~FiniteTimeGenerator() override = default;

  std::pair<Time, bool> Next() override {
    if (next_index_ >= times_.size()) {
      return {0, false};
    }
    return {times_[next_index_++], true};
  }

  std::unique_ptr<TimeGenerator> Clone() override {
    auto clone = std::make_unique<FiniteTimeGenerator>(times_);
    clone->next_index_ = this->next_index_;
    return std::move(clone);
  }

 private:
  std::size_t next_index_ = 0;
  std::vector<Time> times_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_TIME_GENERATOR_H_
