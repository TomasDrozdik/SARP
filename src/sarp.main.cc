//
// sarp.cc
//

#include <fstream>
#include <iostream>
#include <memory>
#include <iomanip>

#include "network_generator/event_generator.h"
#include "sarp/routing.h"
#include "scenarios/basic.h"
#include "scenarios/readdress.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
#ifdef CSV
  std::cout << "run" << ',';
  Parameters::PrintCsvHeader(std::cout);
  Statistics::PrintCsvHeader(std::cout);
#endif
  for (int run = 0; run < 1; ++run) {
    for (double treshold = 1.5; treshold <= 4; treshold += 0.05) {
      Parameters::Sarp sarp_parameters = {
          .neighbor_cost = Cost(1, 0.1),
          .compact_treshold = treshold,
          .update_treshold = 0.9};
      auto [env, network, event_generators] =
          //  LocalStatic(RoutingType::SARP, sarp_parameters);
          //  SpreadOutStatic(RoutingType::SARP, sarp_parameters);
          //  Template(RoutingType::SARP);
          //  LinearThreeNodeStatic(RoutingType::SARP);
          //  LinearStaticOctreeAddresses(RoutingType::SARP, 100, sarp_parameters);
          //  SquareStaticOctreeAddresses(RoutingType::SARP, 10, 10, sarp_parameters);
           CubeStaticOctreeAddresses(RoutingType::SARP, 5, 5, 4, sarp_parameters);
          //  LinearStatic_BinaryAddresses(RoutingType::SARP, 4, sarp_parameters);
          //  LinearThreeNodeSlowMobility(RoutingType::SARP);
          //  TwoNodeGetInRange(RoutingType::SARP);
          //  StaticCube(RoutingType::SARP, sarp_parameters);
          //  MobileCube(RoutingType::SARP, sarp_parameters);
      // SARP SPECIFIC
          //  BootThreeReaddressNew(sarp_parameters);
          // StaticCubeReaddress(sarp_parameters);

#ifdef CSV
    std::cout << run << ',';
#endif
        unsigned seed = std::time(nullptr);
        Simulation::Run(env, *network, seed, event_generators);

#ifdef DUMP
        for (const auto &node : network->get_nodes()) {
          dynamic_cast<const SarpRouting &>(node->get_routing()).Dump(std::cerr);
        }
#endif
    }
  }
  return 0;
}
