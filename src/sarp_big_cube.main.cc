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
    for (double threshold = 3; threshold < 4; threshold += 0.1) {
      Parameters::Sarp sarp_parameters = {.neighbor_cost = Cost(1, 0.1),
                                          .compact_treshold = threshold,
                                          .update_treshold = 0.05};
      auto [sp, network, event_generators] = CubeStaticOctreeAddresses(
          RoutingType::SARP, 10, 10, 10, sarp_parameters);

#ifdef CSV
      std::cout << run << ',';
#endif
      unsigned seed = std::time(nullptr);
      Simulation::Run(seed, std::move(sp), *network, event_generators);

#ifdef DUMP
      for (const auto &node : network->get_nodes()) {
        dynamic_cast<const SarpRouting &>(node->get_routing()).Dump(std::cerr);
      }
#endif
    }
  }
  return 0;
}
