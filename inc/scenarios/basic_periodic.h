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
Template(RoutingType routing);

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_OctreeAddress(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_BinaryAddresses(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_SlowMobility_(RoutingType routing,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
TwoNodeGetInRange(RoutingType routing);

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Local_Static(RoutingType routing,
                          Parameters::Sarp sarp_settings = Parameters::Sarp());

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SpreadOut_Static(RoutingType routing,
                          Parameters::Sarp sarp_settings = Parameters::Sarp());

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
