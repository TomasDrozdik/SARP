//
// event_generator.h
//

#ifndef SARP_SRC_SIMULATION_NETWORK_GENERATOR_EVENT_GENERATOR_H_
#define SARP_SRC_SIMULATION_NETWORK_GENERATOR_EVENT_GENERATOR_H_

#include <vector>
#include <memory>

#include "../structure/events.h"

namespace simulation {

class EventGenerator {
 public:
  virtual std::unique_ptr<Event> operator++() = 0;
};

}  // namespace simulation

#endif  // SARP_SRC_SIMULATION_NETWORK_GENERATOR_EVENT_GENERATOR_H_