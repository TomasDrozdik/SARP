//
// basic_periodic.h
//

#ifndef SARP_SCENARIOS_BASIC_H_
#define SARP_SCENARIOS_BASIC_H_

#include "scenarios/basic_periodic.h"

#include "network_generator/address_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/global_address_update.h"
#include "structure/event.h"
#include "structure/position.h"

namespace simulation {

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Template(RoutingType routing) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.routing_update_period = 10000;
  general.neighbor_update_period = 10000;
  general.boundaries = {Position(0, 0, 0), Position(300, 0, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 3;
  node_generation.routing_type = routing;
  node_generation.initial_addresses = std::make_unique<SequentialAddressGenerator>();
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 0;

  Parameters::Movement movement;
  movement.end = 0;
  movement.step_period = 1000;
  movement.speed_range = {0, 0};
  movement.pause_range = {0, 0};
  movement.directions = std::make_unique<RandomPositionGenerator>(
      general.boundaries.first, general.boundaries.second);

  Parameters::Sarp sarp_parameters;
  sarp_parameters.neighbor_cost = Cost(1, 0.1);
  sarp_parameters.compact_treshold = 3;
  sarp_parameters.update_treshold = 0.9;
  sarp_parameters.min_standard_deviation = 0.1;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  env.parameters.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add some custom events here:
  //std::vector<std::unique_ptr<Event>> custom_events;
  //custom_events.push_back(std::make_unique<SendEvent>(
  //    300000, TimeType::ABSOLUTE, *network->get_nodes().front(),  // SEGFAULT
  //    *network->get_nodes().back(), 73));
  //
  //event_generators.push_back(
  //    std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_OctreeAddress(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp()) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = node_count;
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = general.duration;  // i.e. 1 occurance
  general.boundaries = {Position(0, 0, 0),
                        Position(node_count * general.connection_range, 0, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = node_count;
  node_generation.routing_type = routing;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < node_count; ++i) {
    positions.push_back(Position(general.connection_range * i, 0, 0));
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
        range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
        *network));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_BinaryAddresses(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp()) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = node_count;
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = general.duration;  // i.e. 1 occurance
  general.boundaries = {Position(0, 0, 0),
                        Position(node_count * general.connection_range, 0, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = node_count;
  node_generation.routing_type = routing;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < node_count; ++i) {
    positions.push_back(Position(general.connection_range * i, 0, 0));
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);
  node_generation.initial_addresses =
      std::make_unique<BinaryAddressGenerator>();

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
        range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
        *network));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_SlowMobility(RoutingType routing,
    Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.routing_update_period = 10000;
  general.neighbor_update_period = 10000;
  general.boundaries = {Position(0, 0, 0), Position(150, 0, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 3;
  node_generation.routing_type = routing;
  node_generation.initial_addresses =
      std::make_unique<SequentialAddressGenerator>();
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(75, 0, 0), Position(150, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {200000, 400000};
  traffic.event_count = 100;

  Parameters::Movement movement;
  movement.end = general.duration;
  movement.step_period = 1000;
  movement.speed_range = {0, 0.1};
  movement.pause_range = {0, 0};
  movement.directions = std::make_unique<RandomPositionGenerator>(
      general.boundaries.first, general.boundaries.second);

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  env.parameters.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }
  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);
  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
TwoNodeGetInRange(RoutingType routing) {
  Parameters::General general;
  general.duration = 700000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.routing_update_period = 100000;
  general.neighbor_update_period = 100000;
  general.boundaries = {Position(0, 0, 0), Position(200, 0, 0)};

  Parameters::Traffic traffic;
  traffic.time_range = {50000, 150000};
  traffic.event_count = 10;

  Parameters::Movement movement;
  movement.end = general.duration;
  movement.step_period = 100000;
  movement.speed_range = {1, 1};
  movement.pause_range = {0, 0};
  movement.directions = nullptr;  // All nodes have set directions manualy.

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(Parameters::Sarp());  // Default suffice.
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  std::vector<std::unique_ptr<Event>> custom_events;

  custom_events.push_back(NodeGenerator::CreateBootEvent(
      0, TimeType::ABSOLUTE, *network, routing, Position(0, 0, 0), Address({0}),
      std::make_unique<FinitePositionGenerator>(std::vector({Position(100,0,0)}))));

  custom_events.push_back(NodeGenerator::CreateBootEvent(
      0, TimeType::ABSOLUTE, *network, routing, Position(300, 0, 0), Address({1}),
      std::make_unique<FinitePositionGenerator>(std::vector({Position(100,0,0)}))));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Local_Static(RoutingType routing, Parameters::Sarp sarp_settings) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 200;
  general.routing_update_period = 10000;
  general.neighbor_update_period = general.routing_update_period;
  general.boundaries = {Position(0, 0, 0), Position(300, 300, 300)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 30;
  node_generation.routing_type = routing;
  node_generation.initial_positions = std::make_unique<RandomPositionGenerator>(
      Position(0, 0, 0), Position(300, 300, 300));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_settings);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
          range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
          *network));
  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SpreadOut_Static(RoutingType routing, Parameters::Sarp sarp_settings) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 150;
  general.routing_update_period = 10000;
  general.neighbor_update_period = general.routing_update_period;
  general.boundaries = {Position(0, 0, 0), Position(500, 500, 500)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 100;
  node_generation.routing_type = routing;
  node_generation.initial_positions = std::make_unique<RandomPositionGenerator>(
      Position(0, 0, 0), Position(500, 500, 500));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 10000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_settings);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
          range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
          *network));
  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
