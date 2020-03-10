//
// static_forwarding.cc
//

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "static_routing/routing.h"
#include "structure/simulation.h"
#include "structure/wireless_connection.h"
#include "network_generator/network_generator.h"
#include "network_generator/position_generator.h"
#include "network_generator/event_generator.h"

using namespace simulation;

void ConnectViaRouting(Network &network, std::size_t on_node_idx,
    std::size_t to_node_idx, std::size_t via_node_idx) {
  Node &node = *network.get_nodes()[on_node_idx];
  Node &to_node = *network.get_nodes()[to_node_idx];
  Node &via_node = *network.get_nodes()[via_node_idx];
  dynamic_cast<StaticRouting&>(node.get_routing()).AddRoute(to_node, via_node);
}

int main() {
  NetworkGenerator<SimpleAddress, StaticRouting, WirelessConnection<>> ng;
  FinitePositionGenerator pos_generator(std::vector<Position>{
      Position(0,0,0), Position(100,0,0), Position(200,0,0)});
  auto network = ng.Create(3, pos_generator);

#define EXPORT
#ifdef EXPORT
  std::string filename("network.dot");
  std::ofstream ofs(filename);
  network->ExportToDot(ofs);
#endif  // EXPORT

//#define CYCLE
#ifdef CYCLE
  ConnectViaRouting(*network, 0, 2, 1);
  ConnectViaRouting(*network, 1, 2, 0);
#else  // Connect all but reflexive routes
  ConnectViaRouting(*network, 0, 1, 1);
  ConnectViaRouting(*network, 0, 2, 1);
  ConnectViaRouting(*network, 1, 0, 0);
  ConnectViaRouting(*network, 1, 2, 2);
  ConnectViaRouting(*network, 2, 0, 1);
  ConnectViaRouting(*network, 2, 1, 1);
#endif  // CYCLE

  // Debug print interfaces
  std::cerr << '\n';
  for (const auto &node : network->get_nodes()) {
    std::cerr << *node << '\n';
    for (const auto &iface : node->get_active_interfaces()) {
      std::cerr << *iface << '\n';
    }
  }
  std::cerr << '\n';

  Time simulation_duration = 500000;
  Time trafic_start = 0;
  Time trafic_end = 400000;
  std::size_t event_count = 10;
  bool reflexive_trafic = false;
  std::vector<std::unique_ptr<EventGenerator>> event_generators;
  event_generators.push_back(std::make_unique<TraficGenerator>(trafic_start,
      trafic_end, network->get_nodes(), event_count, reflexive_trafic));

  uint32_t ttl_limit = 16;
  Simulation::set_properties(
      std::move(network), simulation_duration, ttl_limit);
  Simulation::get_instance().Run(event_generators);
  return 0;
}
