//
// readdress.h
//

#ifndef SARP_SCENARIOS_READDRESS_H_
#define SARP_SCENARIOS_READDRESS_H_

#include <memory>
#include <vector>

#include "network_generator/event_generator.h"
#include "structure/network.h"
#include "structure/simulation.h"

namespace simulation {

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
BootThreeReaddressNew(Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
StaticCubeReaddress(Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
AddNewToGrid(unsigned x, unsigned y, unsigned add_count,
             Parameters::Sarp sarp_parameters = Parameters::Sarp());

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
AddNewToCube(unsigned x, unsigned y, unsigned z, unsigned add_count,
             Parameters::Sarp sarp_parameters);

}  // namespace simulation

#endif  // SARP_SCENARIOS_READDRESS_H_
