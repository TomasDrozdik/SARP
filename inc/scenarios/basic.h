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

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Template(RoutingType routing);

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearStaticOctreeAddresses(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SquareStaticOctreeAddresses(
    RoutingType routing, unsigned x, unsigned y,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
CubeStaticOctreeAddresses(
    RoutingType routing, unsigned x, unsigned y, unsigned z,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearStaticBinaryAddresses(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
TwoNodeGetInRange(RoutingType routing);

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LocalStatic(RoutingType routing,
            Parameters::Sarp sarp_settings = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SpreadOutStatic(RoutingType routing,
                Parameters::Sarp sarp_settings = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
StaticCube(RoutingType routing,
           Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
MobileCube(RoutingType routing,
           Parameters::Sarp sarp_parameters = Parameters::Sarp());

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
