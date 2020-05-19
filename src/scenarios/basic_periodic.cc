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
  general.routing_type = routing;
  general.node_count = 3;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.position_boundaries = {Position(0, 0, 0), Position(300, 0, 0)};
  general.initial_addresses = std::make_unique<SequentialAddressGenerator>();
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 0;

  Parameters::Movement movement;
  movement.time_range = {0, 0};
  movement.step_period = 1000;
  movement.speed_range = {0, 0};
  movement.pause_range = {0, 0};
  movement.neighbor_update_period = 0;
  movement.directions = nullptr;  // with nullptr a Random generator is created

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 10000;

  Parameters::Sarp sarp_parameters;
  sarp_parameters.neighbor_cost = Cost(1, 0.1);
  sarp_parameters.reflexive_cost = Cost(0, 0);
  sarp_parameters.treshold = 2;
  sarp_parameters.do_compacting = true;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddMovement(movement);
  env.parameters.AddPeriodicRouting(periodic_routing);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);
  // Add some custom events here:
  std::vector<std::unique_ptr<Event>> custom_events;
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[2], 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_Periodic_OctreeAddress(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp()) {
  Parameters::General general;
  general.routing_type = routing;
  general.node_count = node_count;
  general.duration = 500000;
  general.ttl_limit = node_count;
  general.connection_range = 100;
  general.position_boundaries = {
      Position(0, 0, 0), Position(node_count * general.connection_range, 0, 0)};
  general.initial_addresses = nullptr;

  std::vector<Position> positions;
  for (std::size_t i = 0; i < node_count; ++i) {
    positions.push_back(Position(general.connection_range * i, 0, 0));
  }
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 1000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddPeriodicRouting(periodic_routing);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
          0, 1, 3,  // start, end, period i.e. it happens only once.
          *network));

  std::vector<std::unique_ptr<Event>> custom_events;
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes().front(),
      network->get_nodes().back(), 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
Linear_Static_Periodic_BinaryAddresses(
    RoutingType routing, std::size_t node_count,
    Parameters::Sarp sarp_parameters = Parameters::Sarp()) {
  Parameters::General general;
  general.routing_type = routing;
  general.node_count = node_count;
  general.duration = 500000;
  general.ttl_limit = node_count;
  general.connection_range = 100;
  general.position_boundaries = {
      Position(0, 0, 0), Position(node_count * general.connection_range, 0, 0)};
  general.initial_addresses = std::make_unique<BinaryAddressGenerator>();

  std::vector<Position> positions;
  for (std::size_t i = 0; i < node_count; ++i) {
    positions.push_back(Position(general.connection_range * i, 0, 0));
  }
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 2000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddPeriodicRouting(periodic_routing);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  std::vector<std::unique_ptr<Event>> custom_events;
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes().front(),
      network->get_nodes().back(), 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearThreeNode_SlowMobility_Periodic(RoutingType routing) {
  Parameters::General general;
  general.routing_type = routing;
  general.node_count = 3;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.position_boundaries = {Position(0, 0, 0), Position(150, 0, 0)};
  general.initial_addresses = std::make_unique<SequentialAddressGenerator>();
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(75, 0, 0), Position(150, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {200000, 400000};
  traffic.event_count = 100;

  Parameters::Movement movement;
  movement.time_range = {0, 500000};
  movement.step_period = 1000;
  movement.speed_range = {0, 0.1};
  movement.pause_range = {0, 0};
  movement.neighbor_update_period = 10000;

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 10000;

  Parameters::Sarp sarp_parameters;
  sarp_parameters.neighbor_cost = Cost(1, 0.1);
  sarp_parameters.reflexive_cost = Cost(0, 0);
  sarp_parameters.treshold = 2;
  sarp_parameters.do_compacting = true;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddMovement(movement);
  env.parameters.AddPeriodicRouting(periodic_routing);
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
  general.routing_type = routing;
  general.node_count = 3;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.position_boundaries = {Position(0, 0, 0), Position(150, 0, 0)};
  general.initial_addresses = std::make_unique<SequentialAddressGenerator>();
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(75, 0, 0), Position(150, 0, 0)}));

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 250000;

  Parameters::Sarp sarp_parameters;
  sarp_parameters.neighbor_cost = Cost(1, 0.1);
  sarp_parameters.reflexive_cost = Cost(0, 0);
  sarp_parameters.treshold = 2;
  sarp_parameters.do_compacting = true;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddPeriodicRouting(periodic_routing);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add some custom events here:
  std::vector<std::unique_ptr<Event>> custom_events;
  // This send should fail.
  custom_events.push_back(std::make_unique<SendEvent>(
      100000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[1], 73));
  // Move node 1 in range of node 0.
  custom_events.push_back(std::make_unique<MoveEvent>(
      150000, TimeType::ABSOLUTE, network->get_nodes()[0], *network,
      Position(199, 0, 0)));
  // Now periodic update should happen.
  // This send should be successful.
  custom_events.push_back(std::make_unique<SendEvent>(
      300000, TimeType::ABSOLUTE, network->get_nodes()[0],
      network->get_nodes()[1], 73));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SpreadOut_Static_Periodic(RoutingType routing, Parameters::Sarp sarp_settings) {
  Parameters::General general;
  general.routing_type = routing;
  general.node_count = 100;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 150;
  general.position_boundaries = {Position(0, 0, 0), Position(500, 500, 500)};
  general.initial_addresses = nullptr;
  general.initial_positions = std::make_unique<RandomPositionGenerator>(
      Position(0, 0, 0), Position(500, 500, 500));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 10000;

  Parameters::PeriodicRouting periodic_routing;
  periodic_routing.update_period = 1000;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);
  env.parameters.AddPeriodicRouting(periodic_routing);
  if (routing == RoutingType::SARP) {
    env.parameters.AddSarp(sarp_settings);
  }

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<SarpGlobalAddressUpdatePeriodicGenerator>(
          0, 1, 3,  // start, end, period i.e. it happens only once.
          *network));
  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
