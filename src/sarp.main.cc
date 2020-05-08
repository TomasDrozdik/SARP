//
// sarp.cc
//

#include <fstream>
#include <iostream>
#include <memory>

#include "network_generator/event_generator.h"
#include "scenarios/basic_periodic.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
  Parameters::Sarp sarp_parameters = {
      .neighbor_cost = {.mean = 1, .sd = 0.4, .group_size = 1},
      .reflexive_cost = {.mean = 0, .sd = 0.4, .group_size = 1},
      .treshold = 1.96,
      .do_compacting = false};
  auto [env, network, event_generators] =
      //  SpreadOut_Static_Periodic(RoutingType::SARP, sarp_parameters);
      //  Template(RoutingType::SARP);
      //  LinearThreeNode_Static_Periodic(RoutingType::SARP);
      Linear_Static_Periodic_OctreeAddress(RoutingType::SARP, 10,
                                           sarp_parameters);
  //  Linear_Static_Periodic_BinaryAddresses(RoutingType::SARP, 10,
  //  sarp_parameters);
  //  LinearThreeNode_SlowMobility_Periodic(RoutingType::SARP);
  //  TwoNodeGetInRange(RoutingType::SARP);

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  Simulation::Run(env, *network, event_generators);
  return 0;
}
