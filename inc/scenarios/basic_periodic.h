//
// basic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include <memory>
#include <vector>

#include "structure/simulation_parameters.h"

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_Static_Periodic(RoutingType routing);

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_SlowMobility_Periodic(RoutingType routing);

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
TwoNodeGetInRange(RoutingType routing);

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
