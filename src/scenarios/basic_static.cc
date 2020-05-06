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

namespace simulation {

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes() {
  Parameters::General general;
  general.routing_type = RoutingType::STATIC;
  general.node_count = 3;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.position_boundaries = {Position(0, 0, 0), Position(300, 0, 0)};
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 10;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Connect all but reflexive routes.
  StaticRouting::Connect(*network, 0, 1, 1);
  StaticRouting::Connect(*network, 0, 2, 1);
  StaticRouting::Connect(*network, 1, 0, 0);
  StaticRouting::Connect(*network, 1, 2, 2);
  StaticRouting::Connect(*network, 2, 0, 1);
  StaticRouting::Connect(*network, 2, 1, 1);

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
ThreeNodes_Cycle() {
  Parameters::General general;
  general.routing_type = RoutingType::STATIC;
  general.node_count = 3;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.position_boundaries = {Position(0, 0, 0), Position(300, 0, 0)};
  general.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 10;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddTraffic(traffic);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

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

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation

#endif  // SARP_SCENARIOS_BASIC_H_
