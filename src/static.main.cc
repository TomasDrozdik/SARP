//
// static_forwarding.cc
//

#include <fstream>
#include <iostream>
#include <memory>

#include "scenarios/basic_static.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
  auto [env, network, event_generators] = ThreeNodes();
  // ThreeNodes_Cycle();

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  Simulation::Run(env, *network, event_generators);
  return 0;
}
