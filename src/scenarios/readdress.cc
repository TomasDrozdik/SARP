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
  bool only_empty = true;
  custom_events.push_back(std::make_unique<ReaddressEvent>(301000,
        TimeType::ABSOLUTE, *network, only_empty));
  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

// This one is broken - shows address selection booting problems.
std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
StaticCubeReaddress(Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 4;
  general.connection_range = 100;
  general.routing_update_period = 100000;
  general.neighbor_update_period = 100000;
  general.boundaries = {Position(0, 0, 0), Position(100, 100, 100)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 8;
  node_generation.routing_type = RoutingType::SARP;
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(0, 0, 100), Position(0, 100, 0),
           Position(0, 100, 100), Position(100, 0, 0), Position(100, 0, 100),
           Position(100, 100, 0), Position(100, 100, 100)}));

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddSarp(sarp_parameters);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  event_generators.push_back(
      std::make_unique<ReaddressEventGenerator>(
        range<Time>{0, general.duration}, general.duration / 4, *network));

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
AddNewToGrid(unsigned x, unsigned y, unsigned add_count, Parameters::Sarp sarp_parameters) {
  auto node_count = x * y;
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 1.1 * (x + y + add_count);
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = 1000;
  general.boundaries = {Position(0, 0, 0),
                        Position(x * general.connection_range,
                                 y * general.connection_range, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = node_count;
  node_generation.routing_type = RoutingType::SARP;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < x; ++i) {
    for (std::size_t j = 0; j < y; ++j) {
      positions.push_back(Position(general.connection_range * i,
                                   general.connection_range * j,
                                   0));
    }
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddSarp(sarp_parameters);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<OctreeAddressingEventGenerator>(
        range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
        *network));

  // Boot new nodes after the network is converged.
  event_generators.push_back(std::make_unique<NodeGenerator>(
      *network, add_count, RoutingType::SARP,
      std::make_unique<RandomTimeGenerator>(range<Time>{300000, 301000}),
      std::make_unique<RandomPositionGenerator>(
        range<Position>{Position(0, 0, 0), Position(x * 100, y * 100, 0)}),
      nullptr));

  // Add readdress event of newly added nodes
  std::vector<std::unique_ptr<Event>> custom_events;
  bool only_empty = true;
  custom_events.push_back(std::make_unique<ReaddressEvent>(400000,
        TimeType::ABSOLUTE, *network, only_empty));

  // Add specific traffic to every other node.
  for (NodeID new_node_id = node_count; new_node_id < node_count + add_count; ++new_node_id) {
    for (NodeID id = 0; id < x * y; ++id) {
      custom_events.push_back(std::make_unique<TrafficEvent>(
            490000, TimeType::ABSOLUTE, *network, new_node_id, id));
    }
    event_generators.push_back(
        std::make_unique<CustomEventGenerator>(std::move(custom_events)));
  }

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

std::tuple<Env, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
AddNewToCube(unsigned x, unsigned y, unsigned z, unsigned add_count, Parameters::Sarp sarp_parameters) {
  auto node_count = x * y * z;
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 1.1 * (x + y + z + add_count);
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = 1000;
  general.boundaries = {Position(0, 0, 0),
                        Position(x * general.connection_range,
                                 y * general.connection_range,
                                 z * general.connection_range)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = node_count;
  node_generation.routing_type = RoutingType::SARP;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < x; ++i) {
    for (std::size_t j = 0; j < y; ++j) {
        for (std::size_t k = 0; k < y; ++k) {
          positions.push_back(Position(general.connection_range * i,
                                       general.connection_range * j,
                                       general.connection_range * k));
        }
    }
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Env env;
  env.parameters.AddGeneral(general);
  env.parameters.AddNodeGeneration(std::move(node_generation));
  env.parameters.AddSarp(sarp_parameters);

  auto [network, event_generators] = Simulation::CreateScenario(env.parameters);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(
      std::make_unique<OctreeAddressingEventGenerator>(
        range<Time>{0,1}, 3,  // start, end, period i.e. it happens only once.
        *network));

  // Boot new nodes after the network is converged.
  event_generators.push_back(std::make_unique<NodeGenerator>(
      *network, add_count, RoutingType::SARP,
      std::make_unique<RandomTimeGenerator>(range<Time>{300000, 301000}),
      std::make_unique<RandomPositionGenerator>(
        range<Position>{Position(0, 0, 0), Position(x * 100, y * 100, z * 100)}),
      nullptr));

  // Add readdress event of newly added nodes
  std::vector<std::unique_ptr<Event>> custom_events;
  bool only_empty = true;
  custom_events.push_back(std::make_unique<ReaddressEvent>(400000,
        TimeType::ABSOLUTE, *network, only_empty));

  // Add specific traffic to every other node.
  for (NodeID new_node_id = node_count; new_node_id < node_count + add_count; ++new_node_id) {
    for (NodeID id = 0; id < node_count; ++id) {
      custom_events.push_back(std::make_unique<TrafficEvent>(
            490000, TimeType::ABSOLUTE, *network, new_node_id, id));
    }
    event_generators.push_back(
        std::make_unique<CustomEventGenerator>(std::move(custom_events)));
  }

  return std::make_tuple(std::move(env), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation

