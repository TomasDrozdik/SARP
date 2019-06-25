//
// main.cc
//
#define EXPORT
//#define DEBUG
//#define SLOW_VERSION

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
#ifndef SLOW_VERSION
  Time step_period = 5000;
#else
  Time step_period = 1000;
#endif  // SLOW_VERSION
  double min_speed = 0;  // m/s
  double max_speed = 0.5;  // m/s
  Time min_pause = 0;
  Time max_pause = 100000;
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
  Time routing_update_start = routing_update_period;
  Time routing_update_end = simulation_duration;
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