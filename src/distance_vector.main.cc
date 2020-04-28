//
// distance_vector.cc
//

#include <fstream>
#include <iostream>
#include <memory>

#include "scenarios/basic_periodic.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
  auto [env, network, event_generators] =
      LinearThreeNode_Static_Periodic(RoutingType::DISTANCE_VECTOR);
  // LinearThreeNode_SlowMobility_Periodic(RoutingType::DISTANCE_VECTOR);
  // TwoNodeGetInRange(RoutingType::DISTANCE_VECTOR);

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  Simulation::Run(env, *network, event_generators);

  // TODO: Mabe let the Run return the final network.
  // #ifdef EXPORT
  //   ofs.open("network_final.dot");
  //   Simulation::get_instance().ExportNetworkToDot(ofs);
  //   ofs.close();
  // #endif  // EXPORT

  return 0;
}
