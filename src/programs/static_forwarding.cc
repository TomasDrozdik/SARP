//
// main.cc
//

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "../static_forwarding/static_routing.h"
#include "../network_generator/network_generator.h"
#include "../network_generator/position_generator.h"
#include "../network_generator/event_generator.h"
#include "../network_generator/address_iterator.h"
#include "../structure/event.h"
#include "../structure/network.h"
#include "../structure/node.h"
#include "../structure/simulation.h"
#include "../structure/wireless_connection.h"

using namespace simulation;

void ConnectViaRouting(Network &network, std::size_t on_node_idx, std::size_t to_node_idx, std::size_t via_node_idx) {
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

#define CYCLE
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
#endif

  // Debug print interfaces
  std::cerr << '\n';
  for (const auto &node : network->get_nodes()) {
    std::cerr << *node << '\n';
    for (const auto &iface : node->get_active_connections()) {
      std::cerr << *iface << '\n';
    }
  }
  std::cerr << '\n';

  Time trafic_start = 0;
  Time trafic_end = 400;
  std::size_t event_count = 10;
  bool reflexive_trafic = false;
  std::vector<std::unique_ptr<EventGenerator>> event_generators;
  event_generators.push_back(std::make_unique<TraficGenerator>(trafic_start,
      trafic_end, network->get_nodes(), event_count, reflexive_trafic));

  Time simulation_duration = 500;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac
  Simulation::set_instance(std::move(network), simulation_duration,
      signal_speed_Mbps);
  Simulation::get_instance().Run(event_generators);
  return 0;
}
