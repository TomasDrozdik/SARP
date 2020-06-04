//
// sarp.cc
//

#include <iostream>

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
    for (double compact_threshold = 2; compact_threshold < 5;
         compact_threshold += 0.05) {
      for (double update_threshold = 0; update_threshold < 0.3;
           update_threshold += 0.05) {
        Parameters::Sarp sarp_parameters = {
            .neighbor_cost = Cost(1, 0.1),
            .compact_treshold = compact_threshold,
            .update_treshold = update_threshold};
        auto [sp, network, event_generators] = LinearStaticOctreeAddresses(
            RoutingType::SARP, 100, sarp_parameters);

#ifdef CSV
        std::cout << run << ',';
#endif
        unsigned seed = std::time(nullptr);
        Simulation::Run(seed, std::move(sp), *network, event_generators);

#ifdef DUMP
        for (const auto &node : network->get_nodes()) {
          dynamic_cast<const SarpRouting &>(node->get_routing())
              .Dump(std::cerr);
        }
#endif
      }
    }
  }
  return 0;
}
