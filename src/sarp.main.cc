//
// sarp.cc
//

#include <fstream>
#include <iostream>
#include <memory>
#include <iomanip>

#include "network_generator/event_generator.h"
#include "sarp/routing.h"
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
  double treshold = 3;
  for (int run = 0; run < 1; ++run) {
    Parameters::Sarp sarp_parameters = {
        .neighbor_cost = Cost(1, 0.1),
        .reflexive_cost = Cost(0, 0),
        .max_cost = Cost(std::numeric_limits<double>::max(), 0),
        .compact_treshold = treshold,
        .update_treshold = 0.9};
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

      std::cerr << "ROUTING TABLE DUMP\n";
      for (const auto &node : network->get_nodes()) {
        dynamic_cast<const SarpRouting &>(node.get_routing()).Dump(std::cerr);
      }
    }
  return 0;
}
