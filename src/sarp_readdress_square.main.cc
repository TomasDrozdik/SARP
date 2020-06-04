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
  std::cout << "run" << ',' << "added_nodes" << ',';
  Parameters::PrintCsvHeader(std::cout);
  Statistics::PrintCsvHeader(std::cout);
#endif
  for (int run = 0; run < 100; ++run) {
    for (int add_count = 1; add_count <= 10; ++add_count) {
      Parameters::Sarp sarp_parameters = {.neighbor_cost = Cost(1, 0.1),
                                          .compact_treshold = 5,
                                          .update_treshold = 0.05,
                                          .ratio_variance_treshold = 0.9,
                                          .min_standard_deviation = 0.1};
      auto [sp, network, event_generators] =
          AddNewToGrid(5, 5, add_count, sarp_parameters);

#ifdef CSV
      std::cout << run << ',' << add_count << ',';
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
