//
// basic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include "scenarios/basic_static.h"

#include "network_generator/event_generator.h"
#include "network_generator/position_generator.h"
#include "static_routing/routing.h"
#include "structure/event.h"
#include "structure/position.h"

extern std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes() {
  // General
  RoutingType routing_type = RoutingType::STATIC;
  const uint32_t node_count = 3;
  const Time duration = 500000;
  const uint32_t ttl_limit = 16;
  const uint32_t connection_range = 100;
  // Position boundaries
  const Position position_min = Position(0, 0, 0);
  const Position position_max = Position(300, 0, 0);
  // PositionGenerators
  auto initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  // Traffic generation parameters.
  const Time traffic_start = 300000;
  const Time traffic_end = 400000;
  const std::size_t traffic_event_count = 10;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddTraffic(traffic_start, traffic_end, traffic_event_count);

  // Not nice but f it.
  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  // Connect all but reflexive routes.
  StaticRouting::Connect(*network, 0, 1, 1);
  StaticRouting::Connect(*network, 0, 2, 1);
  StaticRouting::Connect(*network, 1, 0, 0);
  StaticRouting::Connect(*network, 1, 2, 2);
  StaticRouting::Connect(*network, 2, 0, 1);
  StaticRouting::Connect(*network, 2, 1, 1);

  return std::make_pair(std::move(network), std::move(event_generators));
}

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes_Cycle() {
  // General
  RoutingType routing_type = RoutingType::STATIC;
  const uint32_t node_count = 3;
  const Time duration = 500000;
  const uint32_t ttl_limit = 16;
  const uint32_t connection_range = 100;
  // Position boundaries
  const Position position_min = Position(0, 0, 0);
  const Position position_max = Position(300, 0, 0);
  // PositionGenerators
  auto initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  // Traffic generation parameters.
  const Time traffic_start = 300000;
  const Time traffic_end = 400000;
  const std::size_t traffic_event_count = 10;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddTraffic(traffic_start, traffic_end, traffic_event_count);

  // Not nice but f it.
  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  // Connect all but reflexive routes.
  StaticRouting::Connect(*network, 0, 1, 1);
  StaticRouting::Connect(*network, 0, 2, 1);
  StaticRouting::Connect(*network, 1, 0, 0);
  StaticRouting::Connect(*network, 1, 2, 2);
  StaticRouting::Connect(*network, 2, 0, 1);
  StaticRouting::Connect(*network, 2, 1, 1);
  // Add cycle.
  StaticRouting::Connect(*network, 0, 2, 1);
  StaticRouting::Connect(*network, 1, 2, 0);

  return std::make_pair(std::move(network), std::move(event_generators));
}

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
