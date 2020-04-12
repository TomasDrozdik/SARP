//
// sarp.cc
//

#include <fstream>
#include <iostream>

#include "network_generator/event_generator.h"
#include "network_generator/network_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/global_address_update.h"
#include "sarp/routing.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"
#include "structure/position.h"
#include "structure/simulation.h"

using namespace simulation;

int main() {
  SimulationParameters::set_node_count(2);
  SimulationParameters::set_duration(500000);
  SimulationParameters::set_ttl_limit(16);
  SimulationParameters::set_connection_range(1);

  SimulationParameters::set_traffic_start(100000);
  SimulationParameters::set_traffic_end(400000);
  SimulationParameters::set_traffic_event_count(0);

  SimulationParameters::set_move_start(0);
  SimulationParameters::set_move_end(0);
  // SimulationParameters::set_move_end(SimulationParameters::get_duration());
  SimulationParameters::set_step_period(1000);
  SimulationParameters::set_min_speed(0);    // m/s
  SimulationParameters::set_max_speed(0.1);  // m/s
  SimulationParameters::set_min_pause(0);
  SimulationParameters::set_max_pause(0);

  SimulationParameters::set_routing_update_period(250000);
  SimulationParameters::set_routing_update_start(250000);
  SimulationParameters::set_routing_update_end(
      SimulationParameters::get_duration());

  SimulationParameters::set_neighbor_update_period(250000);

  SimulationParameters::set_position_min(Position(0, 0, 0));
  SimulationParameters::set_position_max(Position(100, 100, 100));

#if 0
  auto pos_generator = std::make_unique<RandomPositionGenerator>(
      SimulationParameters::get_position_min(),
      SimulationParameters::get_position_max());
#elif 0
  std::ifstream is("network.dot");
  auto pos_generator = std::make_unique<FinitePositionGenerator>(is);
#else
  auto pos_generator = std::make_unique<FinitePositionGenerator>(
      std::vector({Position(0, 0, 0), Position(0, 0, 0)}));
  //{Position(0, 0, 0), Position(100, 0, 0), Position(0, 100, 0),
  // Position(0, 0, 100), Position(100, 100, 0), Position(0, 100, 100),
  // Position(100, 0, 100), Position(100, 100, 100), Position(50,50,50) }));
#endif

  NetworkGenerator<SarpRouting> ng;
  auto network =
      ng.Create(SimulationParameters::get_node_count(),
                std::move(pos_generator), SequentialAddressGenerator());

//#define EXPORT
#ifdef EXPORT
  std::ofstream ofs("network_initial.dot");
  network->ExportToDot(ofs);
  ofs.close();
#endif  // EXPORT

  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  event_generators.push_back(std::make_unique<TrafficGenerator>(
      SimulationParameters::get_traffic_start(),
      SimulationParameters::get_traffic_end(), network->get_nodes(),
      SimulationParameters::get_traffic_event_count()));

#if 1
  RandomPositionGenerator destination_generator(
      SimulationParameters::get_position_min(),
      SimulationParameters::get_position_max());
#else
  FinitePositionGenerator destination_generator(std::vector<Position>{
      Position(0, 50, 0), Position(100, 0, 0), Position(200, 0, 0)});
#endif
  event_generators.push_back(std::make_unique<MoveGenerator>(
      SimulationParameters::get_move_start(),
      SimulationParameters::get_move_end(),
      SimulationParameters::get_step_period(), *network,
      std::make_unique<RandomPositionGenerator>(
          SimulationParameters::get_position_min(),
          SimulationParameters::get_position_max()),
      SimulationParameters::get_min_speed(),
      SimulationParameters::get_max_speed(),
      SimulationParameters::get_min_pause(),
      SimulationParameters::get_max_pause()));

  event_generators.push_back(std::make_unique<NeighborPeriodicUpdateGenerator>(
      SimulationParameters::get_neighbor_update_period(),
      SimulationParameters::get_duration(), *network));

  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
          0, 250001, 250000, *network));

  Simulation::get_instance().Run(std::move(network),
                                 std::move(event_generators));

#ifdef EXPORT
  ofs.open("network_final.dot");
  Simulation::get_instance().ExportNetworkToDot(ofs);
  ofs.close();
#endif  // EXPORT
  return 0;
}
