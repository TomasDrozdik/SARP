//
// main.cc
//

#include <iostream>

#include "../distance_vector_routing/dv_routing.h"
#include "../network_generator/network_generator.h"
#include "../network_generator/position_generator.h"
#include "../network_generator/event_generator.h"
#include "../network_generator/address_iterator.h"
#include "../structure/event.h"
#include "../structure/network.h"
#include "../structure/node.h"
#include "../structure/position.h"
#include "../structure/simulation.h"
#include "../structure/wireless_connection.h"

using namespace simulation;

int main() {
  NetworkGenerator<SimpleAddress, DistanceVectorRouting,
      WirelessConnection<>> ng;
  //RandomPositionGenerator pos_generator(0, 500);
  FinitePositionGenerator pos_generator(std::vector<Position>{
      Position(0,0,0), Position(100,0,0), Position(200,0,0), Position(300,0,0),
      Position(400,0,0), Position(500,0,0), Position(600,0,0)});
  auto network = ng.Create(7, pos_generator);

  // Debug print interfaces
  std::cerr << '\n';
  for (const auto &node : network->get_nodes()) {
    std::cerr << *node << '\n';
    for (const auto &iface : node->get_active_connections()) {
      std::cerr << *iface << '\n';
    }
  }
  std::cerr << '\n';

  Time simulation_duration = 500000;
  Time trafic_start = 300000;
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