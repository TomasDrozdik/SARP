//
// move_generator.h
//

#ifndef SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFFIC_GENERATOR_H_
#define SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFFIC_GENERATOR_H_

#include "../structure/event.h"

#include <vector>
#include <memory>

namespace simulation {

class MoveGenerator : public EventGenerator {
 public:
  MoveGenerator(const Network &incomplete_network);
  std::unique_ptr<Event> operator++() override;
};

}  // namespace simulation

#endif  // SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFFIC_GENERATOR_H_
