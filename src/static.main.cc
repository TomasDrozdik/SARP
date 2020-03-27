//
// static_forwarding.cc
//

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "network_generator/address_generator.h"
#include "network_generator/event_generator.h"
#include "network_generator/network_generator.h"
#include "network_generator/position_generator.h"
#include "static_routing/routing.h"
#include "structure/simulation.h"
#include "structure/simulation_parameters.h"

using namespace simulation;

void ConnectViaRouting(Network &network, std::size_t on_node_idx,
                       std::size_t to_node_idx, std::size_t via_node_idx) {
  Node &node = *network.get_nodes()[on_node_idx];
  Node &to_node = *network.get_nodes()[to_node_idx];
  Node &via_node = *network.get_nodes()[via_node_idx];
  dynamic_cast<StaticRouting &>(node.get_routing()).AddRoute(to_node, via_node);
}

int main() {
  SimulationParameters::set_duration(500000);
  SimulationParameters::set_ttl_limit(16);
  SimulationParameters::set_connection_range(100);

  SimulationParameters::set_traffic_start(0);
  SimulationParameters::set_traffic_end(400000);
  SimulationParameters::set_traffic_event_count(10);
  SimulationParameters::set_reflexive_traffic(false);

  NetworkGenerator<StaticRouting> ng;
  auto network = ng.Create(
      3,
      std::make_unique<FinitePositionGenerator>(std::vector<Position>{
          Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}),
      SequentialAddressGenerator());

//#define EXPORT
#ifdef EXPORT
  std::string filename("network.dot");
  std::ofstream ofs(filename);
  network->ExportToDot(ofs);
#endif  // EXPORT

//#define CYCLE
#ifdef CYCLE
  ConnectViaRouting(*network, 0, 2, 1);
  ConnectViaRouting(*network, 1, 2, 0);
#else   // Connect all but reflexive routes
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

  // TODO: add these to simulation parameters
  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  std::vector<std::unique_ptr<Event>> custom_events;
  custom_events.push_back(std::make_unique<SendEvent>(
      100, TimeType::ABSOLUTE, *network->get_nodes()[2],
      *network->get_nodes()[0], 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  event_generators.push_back(std::make_unique<TrafficGenerator>(
      SimulationParameters::get_traffic_start(),
      SimulationParameters::get_traffic_end(), network->get_nodes(),
      SimulationParameters::get_traffic_event_count(),
      SimulationParameters::get_reflexive_traffic()));

  Simulation::get_instance().Run(std::move(network),
                                 std::move(event_generators));
  return 0;
}
