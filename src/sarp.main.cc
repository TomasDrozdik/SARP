//
// sarp.cc
//

#include <fstream>
#include <iostream>
#include <memory>
#include <iomanip>

#include "network_generator/event_generator.h"
#include "scenarios/basic_periodic.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
#ifdef CSV
  std::cout << std::setw(W) << "run" << ',';
  Parameters::PrintCsvHeader(std::cout);
  Statistics::PrintCsvHeader(std::cout);
#endif
  for (double var = 0.5; var <= 0.5; var += 0.001) {
    for (int run = 0; run < 1; ++run) {
      Parameters::Sarp sarp_parameters = {
          .neighbor_cost = {.mean = 1, .var = var, .group_size = 1},
          .reflexive_cost = {.mean = 0, .var = var, .group_size = 1},
          .treshold = 1.96};
      auto [env, network, event_generators] =
          //  SpreadOut_Static_Periodic(RoutingType::SARP, sarp_parameters);
          //  Template(RoutingType::SARP);
          //  LinearThreeNode_Static_Periodic(RoutingType::SARP);
              Linear_Static_Periodic_OctreeAddress(RoutingType::SARP, 100, sarp_parameters);
          //  Linear_Static_Periodic_BinaryAddresses(RoutingType::SARP, 4, sarp_parameters);
          //  LinearThreeNode_SlowMobility_Periodic(RoutingType::SARP);
          //  TwoNodeGetInRange(RoutingType::SARP);
#ifdef CSV
      std::cout << std::setw(W) << run << ',';
#endif
      Simulation::Run(env, *network, event_generators);
    }
  }
  return 0;
}
