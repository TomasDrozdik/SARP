//
// basic.cc
//

#include "scenarios/basic.h"

#include "network_generator/address_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/octree.h"
#include "structure/event.h"
#include "structure/position.h"
#include "structure/simulation.h"

namespace simulation {

std::tuple<Parameters, std::unique_ptr<Network>,
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
  node_generation.initial_addresses =
      std::make_unique<SequentialAddressGenerator>();
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
  movement.directions =
      std::make_unique<RandomPositionGenerator>(general.boundaries);

  Parameters::Sarp sarp_parameters;
  sarp_parameters.neighbor_cost = Cost(1, 0.1);
  sarp_parameters.compact_treshold = 3;
  sarp_parameters.update_treshold = 0.9;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  sp.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add some custom events here:
  // std::vector<std::unique_ptr<Event>> custom_events;
  // custom_events.push_back(std::make_unique<SendEvent>(
  //    300000, TimeType::ABSOLUTE, *network->get_nodes().front(),  // SEGFAULT
  //    *network->get_nodes().back(), 73));
  //
  // event_generators.push_back(
  //    std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LinearStaticOctreeAddresses(RoutingType routing, std::size_t node_count,
                            Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 1.1 * node_count;
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
  traffic.time_range = {400000, 450000};
  traffic.event_count = 10000;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));

  event_generators.push_back(std::make_unique<SpecificTrafficGenerator>(
      range<Time>{400000, 450000}, *network, 4, range<NodeID>{2, 3},
      range<NodeID>{0, 1}));

  //  std::vector<std::unique_ptr<Event>> custom_events;
  //  custom_events.push_back(std::make_unique<TrafficEvent>(
  //      300000, TimeType::ABSOLUTE, *network, 0, node_count - 1));
  //  event_generators.push_back(
  //      std::make_unique<CustomEventGenerator>(std::move(custom_events)));
  //
  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SquareStaticOctreeAddresses(RoutingType routing, unsigned x, unsigned y,
                            Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 1.1 * (x + y);
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = general.duration;  // i.e. 1 occurance
  general.boundaries = {
      Position(0, 0, 0),
      Position(x * general.connection_range, y * general.connection_range, 0)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = x * y;
  node_generation.routing_type = routing;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < x; ++i) {
    for (std::size_t j = 0; j < y; ++j) {
      positions.push_back(Position(general.connection_range * i,
                                   general.connection_range * j, 0));
    }
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Parameters::Traffic traffic;
  traffic.time_range = {400000, 450000};
  traffic.event_count = 10000;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));

  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
CubeStaticOctreeAddresses(RoutingType routing, unsigned x, unsigned y,
                          unsigned z, Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 1.1 * (x + y + z);
  general.connection_range = 100;
  general.routing_update_period = 1000;
  general.neighbor_update_period = general.duration;  // i.e. 1 occurance
  general.boundaries = {
      Position(0, 0, 0),
      Position(x * general.connection_range, y * general.connection_range,
               z * general.connection_range)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = x * y * z;
  node_generation.routing_type = routing;
  std::vector<Position> positions;
  for (std::size_t i = 0; i < x; ++i) {
    for (std::size_t j = 0; j < y; ++j) {
      for (std::size_t k = 0; k < z; ++k) {
        positions.push_back(Position(general.connection_range * i,
                                     general.connection_range * j,
                                     general.connection_range * k));
      }
    }
  }
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(positions);

  Parameters::Traffic traffic;
  traffic.time_range = {400000, 450000};
  traffic.event_count = 10000;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));

  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
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

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddTraffic(traffic);
  sp.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    sp.AddSarp(Parameters::Sarp());  // Default suffice.
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  std::vector<std::unique_ptr<Event>> custom_events;

  custom_events.push_back(NodeGenerator::CreateBootEvent(
      0, TimeType::ABSOLUTE, *network, routing, Position(0, 0, 0), Address({0}),
      std::make_unique<FinitePositionGenerator>(
          std::vector({Position(100, 0, 0)}))));

  custom_events.push_back(
      NodeGenerator::CreateBootEvent(0, TimeType::ABSOLUTE, *network, routing,
                                     Position(300, 0, 0), Address({1}),
                                     std::make_unique<FinitePositionGenerator>(
                                         std::vector({Position(100, 0, 0)}))));

  event_generators.push_back(
      std::make_unique<CustomEventGenerator>(std::move(custom_events)));

  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
LocalStatic(RoutingType routing, Parameters::Sarp sarp_settings) {
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
      range<Position>{Position(0, 0, 0), Position(300, 300, 300)});

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 1000;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_settings);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));
  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
SpreadOutStatic(RoutingType routing, Parameters::Sarp sarp_settings) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 16;
  general.connection_range = 150;
  general.routing_update_period = 1000;
  general.neighbor_update_period = general.routing_update_period;
  general.boundaries = {Position(0, 0, 0), Position(500, 500, 500)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 100;
  node_generation.routing_type = routing;
  node_generation.initial_positions = std::make_unique<RandomPositionGenerator>(
      range<Position>{Position(0, 0, 0), Position(500, 500, 500)});

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 10000;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_settings);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  // Add global initial addressing, happens only once at a start
  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));
  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
StaticCube(RoutingType routing, Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 4;
  general.connection_range = 100;
  general.routing_update_period = 10000;
  general.neighbor_update_period = 10000;
  general.boundaries = {Position(0, 0, 0), Position(100, 100, 100)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 8;
  node_generation.routing_type = routing;
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(0, 0, 100), Position(0, 100, 0),
           Position(0, 100, 100), Position(100, 0, 0), Position(100, 0, 100),
           Position(100, 100, 0), Position(100, 100, 100)}));

  Parameters::Traffic traffic;
  traffic.time_range = {300000, 400000};
  traffic.event_count = 100;

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);
  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

std::tuple<Parameters, std::unique_ptr<Network>,
           std::vector<std::unique_ptr<EventGenerator>>>
MobileCube(RoutingType routing, Parameters::Sarp sarp_parameters) {
  Parameters::General general;
  general.duration = 500000;
  general.ttl_limit = 4;
  general.connection_range = 100;
  general.routing_update_period = 10000;
  general.neighbor_update_period = 10000;
  general.boundaries = {Position(0, 0, 0), Position(900, 900, 900)};

  Parameters::NodeGeneration node_generation;
  node_generation.node_count = 8;
  node_generation.routing_type = routing;
  node_generation.initial_positions =
      std::make_unique<FinitePositionGenerator>(std::vector(
          {Position(0, 0, 0), Position(0, 0, 100), Position(0, 100, 0),
           Position(0, 100, 100), Position(100, 0, 0), Position(100, 0, 100),
           Position(100, 100, 0), Position(100, 100, 100)}));

  Parameters::Traffic traffic;
  traffic.time_range = {200000, 400000};
  traffic.event_count = 1000;

  Parameters::Movement movement;
  movement.end = general.duration;
  movement.step_period = 1000;
  movement.speed_range = {1, 1};
  movement.pause_range = {0, 0};
  movement.directions = std::make_unique<FinitePositionGenerator>(std::vector(
      {Position(0, 0, 0), Position(0, 0, 900), Position(0, 900, 0),
       Position(0, 900, 900), Position(900, 0, 0), Position(900, 0, 900),
       Position(900, 900, 0), Position(900, 900, 900)}));

  Parameters sp;
  sp.AddGeneral(general);
  sp.AddNodeGeneration(std::move(node_generation));
  sp.AddTraffic(traffic);
  sp.AddMovement(std::move(movement));
  if (routing == RoutingType::SARP) {
    sp.AddSarp(sarp_parameters);
  }

  auto [network, event_generators] = Simulation::CreateScenario(sp);

  event_generators.push_back(std::make_unique<OctreeAddressingEventGenerator>(
      range<Time>{0, 1}, 3,  // start, end, period i.e. it happens only once.
      *network));

  return std::make_tuple(std::move(sp), std::move(network),
                         std::move(event_generators));
}

}  // namespace simulation
