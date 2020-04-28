//
// basic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include <memory>
#include <vector>

#include "network_generator/event_generator.h"
#include "structure/network.h"
#include "structure/simulation.h"

namespace simulation {

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes();

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes_Cycle();

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
