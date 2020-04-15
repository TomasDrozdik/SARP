//
// basic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include "scenarios/basic_periodic.h"

#include "network_generator/event_generator.h"
#include "network_generator/position_generator.h"
#include "structure/event.h"
#include "structure/position.h"

extern std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
Templace(RoutingType routing) {
  // General
  RoutingType routing_type = routing;
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

  // Movement simulation parameters.
  const Time move_start = 0;
  const Time move_end = 0;
  const Time move_step_period = 1000;
  const double move_speed_min = 0;    // m/s
  const double move_speed_max = 0.1;  // m/s
  const Time move_pause_min = 0;
  const Time move_pause_max = 0;
  // Periodic neighbor update parameters.
  const Time neighbor_update_period = 100000;

  // Periodic routing update parameters.
  const Time routing_update_period = 100000;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddTraffic(traffic_start, traffic_end, traffic_event_count)
      .AddRandomMovement(move_start, move_end, move_step_period, move_speed_min,
                         move_speed_max, move_pause_min, move_pause_max,
                         neighbor_update_period)
      .AddPeriodicRoutingUpdate(routing_update_period);

  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  // Add some custom events here:
  std::vector<std::unique_ptr<Event>> custom_events;
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[2], 73));
  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_pair(std::move(network), std::move(event_generators));
}

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_Static_Periodic(RoutingType routing) {
  // General
  RoutingType routing_type = routing;
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
  //
  // Periodic routing update parameters.
  const Time routing_update_period = 100000;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddTraffic(traffic_start, traffic_end, traffic_event_count)
      .AddPeriodicRoutingUpdate(routing_update_period);

  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  return std::make_pair(std::move(network), std::move(event_generators));
}

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_SlowMobility_Periodic(RoutingType routing) {
  // General
  RoutingType routing_type = routing;
  const uint32_t node_count = 3;
  const Time duration = 500000;
  const uint32_t ttl_limit = 16;
  const uint32_t connection_range = 100;
  // Position boundaries
  const Position position_min = Position(0, 0, 0);
  const Position position_max = Position(250, 0, 0);
  // PositionGenerators
  auto initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  // Traffic generation parameters.
  const Time traffic_start = 200000;
  const Time traffic_end = 400000;
  const std::size_t traffic_event_count = 100;

  // Movement simulation parameters.
  const Time move_start = 0;
  const Time move_end = 500000;
  const Time move_step_period = 1000;
  const double move_speed_min = 0;    // m/s
  const double move_speed_max = 0.1;  // m/s
  const Time move_pause_min = 0;
  const Time move_pause_max = 0;
  // Periodic neighbor update parameters.
  const Time neighbor_update_period = 100000;

  // Periodic routing update parameters.
  const Time routing_update_period = 100000;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddTraffic(traffic_start, traffic_end, traffic_event_count)
      .AddRandomMovement(move_start, move_end, move_step_period, move_speed_min,
                         move_speed_max, move_pause_min, move_pause_max,
                         neighbor_update_period)
      .AddPeriodicRoutingUpdate(routing_update_period);

  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  return std::make_pair(std::move(network), std::move(event_generators));
}

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
TwoNodeGetInRange(RoutingType routing) {
  // General
  RoutingType routing_type = routing;
  const uint32_t node_count = 2;
  const Time duration = 500000;
  const uint32_t ttl_limit = 16;
  const uint32_t connection_range = 100;
  // Position boundaries
  const Position position_min = Position(0, 0, 0);
  const Position position_max = Position(250, 0, 0);
  // PositionGenerators
  auto initial_positions = std::make_unique<FinitePositionGenerator>(
      std::vector({Position(0, 0, 0), Position(102, 0, 0)}));

  // Periodic routing update parameters.
  const Time routing_update_period = 250000;

  SimulationParametersBuilder spb(routing_type, node_count, duration, ttl_limit,
                                  connection_range, position_min, position_max,
                                  std::move(initial_positions));

  spb.AddPeriodicRoutingUpdate(routing_update_period);

  config = spb.Build();

  auto [network, event_generators] = Simulation::CreateScenario();

  // Add some custom events here:
  std::vector<std::unique_ptr<Event>> custom_events;
  // This send should fail.
  custom_events.push_back(std::make_unique<SendEvent>(
      100000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[1], 73));
  // Now move node 1 in range of node 0.
  custom_events.push_back(std::make_unique<MoveEvent>(
      150000, TimeType::ABSOLUTE, network->get_nodes()[0], *network,
      Position(100, 0, 0)));
  // Now periodic update should happen.
  // Now this send should be successful.
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[1], 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_pair(std::move(network), std::move(event_generators));
}

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
