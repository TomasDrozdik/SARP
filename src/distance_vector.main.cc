//
// distance_vector.cc
//

#include <fstream>
#include <iostream>
#include <memory>

#include "scenarios/basic.h"
#include "structure/network.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
#ifdef CSV
  Parameters::PrintCsvHeader(std::cout);
  Statistics::PrintCsvHeader(std::cout);
#endif
  auto [sp, network, event_generators] =
      // LocalStatic(RoutingType::DISTANCE_VECTOR);
      // LinearStaticOctreeAddress(RoutingType::DISTANCE_VECTOR, 100);
      // SquareStaticOctreeAddresses(RoutingType::DISTANCE_VECTOR, 10, 10);
      CubeStaticOctreeAddresses(RoutingType::DISTANCE_VECTOR, 5, 5, 5);
  // LinearStaticBinaryAddresses(RoutingType::DISTANCE_VECTOR, 10);
  // Template(RoutingType::DISTANCE_VECTOR);
  // LinearThreeNodeStatic(RoutingType::DISTANCE_VECTOR);
  // LinearThreeNodeSlowMobility(RoutingType::DISTANCE_VECTOR);
  // TwoNodeGetInRange(RoutingType::DISTANCE_VECTOR);
  // StaticCube(RoutingType::DISTANCE_VECTOR);
  // MobileCube(RoutingType::DISTANCE_VECTOR);

#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  unsigned seed = std::time(nullptr);
  Simulation::Run(seed, std::move(sp), *network, event_generators);

#ifdef EXPORT
  ofs.open("network_final.dot");
  Simulation::get_instance().ExportNetworkToDot(ofs);
  ofs.close();
#endif  // EXPORT

  return 0;
}
