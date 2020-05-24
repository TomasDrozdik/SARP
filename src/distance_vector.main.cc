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
  //  Local_Static_Periodic(RoutingType::DISTANCE_VECTOR);
  //  Linear_Static_Periodic_OctreeAddress(RoutingType::DISTANCE_VECTOR, 100);
  //  Linear_Static_Periodic_BinaryAddresses(RoutingType::DISTANCE_VECTOR, 10);
  // Template(RoutingType::DISTANCE_VECTOR);
  // LinearThreeNode_Static_Periodic(RoutingType::DISTANCE_VECTOR);
  // LinearThreeNode_SlowMobility_Periodic(RoutingType::DISTANCE_VECTOR);
   TwoNodeGetInRange(RoutingType::DISTANCE_VECTOR);

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT
  unsigned seed = std::time(nullptr);
  Simulation::Run(env, *network, seed, event_generators);

  // TODO: Mabe let the Run return the final network.
  // #ifdef EXPORT
  //   ofs.open("network_final.dot");
  //   Simulation::get_instance().ExportNetworkToDot(ofs);
  //   ofs.close();
  // #endif  // EXPORT

  return 0;
}
