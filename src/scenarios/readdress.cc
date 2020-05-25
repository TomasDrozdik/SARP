//
// readdress.cc
//

#include "scenarios/readdress.h"

#include "network_generator/address_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/octree.h"
#include "structure/event.h"
#include "structure/position.h"

namespace simulation {

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
BootThreeReaddressNew(Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 100;
  general.routing_update_period = 100000;
  general.neighbor_update_period = 100000;
  general.boundaries = {Position(0, 0, 0), Position(200, 100, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 4;
  node_generation.routing_type = RoutingType::SARP;
  node_generation.boot_time =
      std::make_unique<FiniteTimeGenerator>(std::vector<Time>(
          {0, 0, 0, 200000}));
  node_generation.initial_addresses = nullptr;
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector<Position>(
          {Position(0, 0, 0), Position(100, 0, 0), Position(200, 0, 0),
           Position(100, 100, 0)}));

  Parameters::Traffic traffic;
  traffic.time_range = {100000, 400000};
  traffic.event_count = 0;

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddTraffic(traffic);
  env.parameters.AddSarp(sarp_parameters);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<OctreeAddressingEventGenerator>(
        range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
        *network));

  std::vector<std::unique_ptr<Event>> custom_events;
  bool delete_existing = false;
  bool only_empty = true;
  custom_events.push_back(std::make_unique<ReaddressEvent>(301000,
        TimeType::ABSOLUTE, *network, only_empty, delete_existing));
  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation

