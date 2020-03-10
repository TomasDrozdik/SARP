//
// distance_vector.cc
//
#define EXPORT

#include <fstream>
#include <iostream>

#include "distance_vector/routing.h"
#include "network_generator/network_generator.h"
#include "network_generator/position_generator.h"
#include "network_generator/event_generator.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"
#include "structure/position.h"
#include "structure/simulation.h"
#include "structure/wireless_connection.h"

using namespace simulation;

int main() {
  Position::set_min(Position());
  Position::set_max(Position(500, 500, 500));
  NetworkGenerator<SimpleAddress, DistanceVectorRouting, WirelessConnection> ng;
#if 1
  RandomPositionGenerator pos_generator;
#elif 0
  std::ifstream is("network.dot");
  FinitePositionGenerator pos_generator(is);
#else
  FinitePositionGenerator pos_generator(std::vector<Position>{
      Position(0,0,0), Position(100,0,0), Position(200,0,0)});
#endif
  auto network = ng.Create(100, pos_generator);

#ifdef EXPORT
    std::ofstream ofs("network_initial.dot");
    network->ExportToDot(ofs);
    ofs.close();
#endif  // EXPORT

  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  Time simulation_duration = 750000;
  Time trafic_start = 300000;
  Time trafic_end = 500000 ;
  std::size_t event_count = 1000;
  bool reflexive_trafic = false;
  event_generators.push_back(std::make_unique<TraficGenerator>(trafic_start,
      trafic_end, network->get_nodes(), event_count, reflexive_trafic));

  Time move_start = 0;
  Time move_end = simulation_duration;
  Time step_period = 1000;
  double min_speed = 0;  // m/s
  double max_speed = 0.5;  // m/s
  Time min_pause = 0;
  Time max_pause = 100000;
#if 1
  RandomPositionGenerator destination_generator;
#else
  FinitePositionGenerator destination_generator(std::vector<Position>{
      Position(0,50,0), Position(100,0,0), Position(200,0,0)});
#endif
  event_generators.push_back(std::make_unique<MoveGenerator>(move_start,
      move_end, step_period, *network, destination_generator, min_speed,
      max_speed, min_pause, max_pause));

  Time routing_update_period = 16000;
  Time routing_update_start = routing_update_period;
  Time routing_update_end = simulation_duration;
  event_generators.push_back(std::make_unique<RoutingPeriodicUpdateGenerator>(
      routing_update_start, routing_update_end, routing_update_period,
      *network));

  uint32_t ttl_limit = 16;
  uint32_t connection_range = 100;
  SimulationParameters parameters(simulation_duration, ttl_limit, connection_range);
  Simulation::set_properties(std::move(network), parameters);
  Simulation::get_instance().Run(event_generators);

#ifdef EXPORT
    ofs.open("network_final.dot");
    Simulation::get_instance().get_network().ExportToDot(ofs);
    ofs.close();
#endif  // EXPORT
  return 0;
}
