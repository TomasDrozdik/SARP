//
// basic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include <memory>
#include <vector>

#include "network_generator/event_generator.h"

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes();

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes_Cycle();

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
