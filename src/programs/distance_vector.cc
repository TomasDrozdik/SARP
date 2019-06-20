//
// main.cc
//
#define EXPORT
//#define DEBUG

#include <fstream>
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
      WirelessConnection<250>> ng;
#if 1
  RandomPositionGenerator pos_generator(Position(), Position (1000,1000,0));
#elif 0
  std::ifstream is("network.dot");
  FinitePositionGenerator pos_generator(is);
#else
  FinitePositionGenerator pos_generator(std::vector<Position>{
      Position(0,0,0), Position(100,0,0), Position(200,0,0)});
#endif
  auto network = ng.Create(64, pos_generator);

#ifdef EXPORT
    std::ofstream ofs("network_initial.dot");
    network->ExportToDot(ofs);
    ofs.close();
#endif  // EXPORT

#ifdef DEBUG
  // Debug print interfaces
  std::cerr << "Interfaces\n";
  for (const auto &node : network->get_nodes()) {
    std::cerr << *node << '\n';
    for (const auto &iface : node->get_active_interfaces()) {
      std::cerr << *iface << '\n';
    }
  }
  std::cerr << '\n';
#endif  // DEBUG

#ifdef DEBUG
  // Debug print initial positions
  std::cerr << "\nPositions\n" ;
  for (const auto &node : network->get_nodes()) {
    std::cerr << *node << node->get_connection().position << '\n';
  }
  std::cerr << '\n';
#endif  // DEBUG

  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  Time simulation_duration = 750000;
  Time trafic_start = 30000;
  Time trafic_end = 50000 ;
  std::size_t event_count = 10000;
  bool reflexive_trafic = false;
  event_generators.push_back(std::make_unique<TraficGenerator>(trafic_start,
      trafic_end, network->get_nodes(), event_count, reflexive_trafic));

  Time move_start = 0;
  Time move_end = simulation_duration;
  Time step_period = 1000;
  double min_speed = 0.5;  // m/s
  double max_speed = 5;  // m/s
  Time min_pause = 0;
  Time max_pause = 1000;
#if 1
  RandomPositionGenerator destination_generator(Position(), Position (1000,1000,0));
#else
  FinitePositionGenerator destination_generator(std::vector<Position>{
      Position(0,50,0), Position(100,0,0), Position(200,0,0)});
#endif
  event_generators.push_back(std::make_unique<MoveGenerator>(move_start,
      move_end, step_period, *network, destination_generator, min_speed,
      max_speed, min_pause, max_pause));

  Time routing_update_period = 16000;
  Time routing_update_start = 0 + routing_update_period;
  Time routing_update_end = 3 * routing_update_period;
  event_generators.push_back(std::make_unique<RoutingPeriodicUpdateGenerator>(
      routing_update_start, routing_update_end, routing_update_period,
      *network));

  uint32_t ttl_limit = 18;
  Simulation::set_properties(
      std::move(network), simulation_duration, ttl_limit);
  Simulation::get_instance().Run(event_generators);

#ifdef EXPORT
    ofs.open("network_final.dot");
    Simulation::get_instance().get_network().ExportToDot(ofs);
    ofs.close();
#endif  // EXPORT
  return 0;
}