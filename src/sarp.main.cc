//
// sarp.cc
//

#include <fstream>
#include <iostream>
#include <memory>

#include "network_generator/event_generator.h"
#include "sarp/global_address_update.h"
#include "scenarios/basic_periodic.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
  auto [network, event_generators] =
      // LinearThreeNode_Static_Periodic(RoutingType::SARP);
      LinearThreeNode_SlowMobility_Periodic(RoutingType::SARP);
  // TwoNodeGetInRange(RoutingType::SARP);

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  Simulation::get_instance().Run(std::move(network),
                                 std::move(event_generators));

  // Add global addressing, happens only once at a start.
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(0, 1, 3,
                                                                 *network));

#ifdef EXPORT
  ofs.open("network_final.dot");
  Simulation::get_instance().ExportNetworkToDot(ofs);
  ofs.close();
#endif  // EXPORT

  return 0;
}
