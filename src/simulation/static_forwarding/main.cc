//
// main.cc
//
#include <map>

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "static_routing.h"
#include "../network_generator/trafic_generator.h"
#include "../structure/events.h"
#include "../structure/network.h"
#include "../structure/node.h"
#include "../structure/node.h"
#include "../structure/position.h"
#include "../structure/simulation.h"
#include "../structure/wireless_connection.h"

using namespace simulation;

std::unique_ptr<Node> CreateNode(
    std::unique_ptr<Address> addr, Position pos) {
  auto node = std::make_unique<Node>();
  node->add_address(std::move(addr));

  auto connection = std::make_unique<WirelessConnection>(*node, pos);
  node->set_connection(std::move(connection));

  auto routing = std::make_unique<StaticRouting>(*node);
  node->set_routing(std::move(routing));

  assert(node->IsInitialized());
  return std::move(node);
}

void ConnectViaRouting(Node &node, const Node &to_node) {
  dynamic_cast<StaticRouting&>(node.get_routing()).ConnectToNode(to_node);
}

std::unique_ptr<Network> CreateSimpleNetwork() {
  auto nodes = std::make_unique<std::vector<std::unique_ptr<Node>>>();
  nodes->push_back(CreateNode(std::make_unique<SimpleAddress>(0),
      Position(0, 0, 0)));

  nodes->push_back(CreateNode(std::make_unique<SimpleAddress>(1),
      Position(1, 1, 1)));

  nodes->push_back(CreateNode(std::make_unique<SimpleAddress>(2),
      Position(2, 2, 2)));

  for (std::size_t i = 0; i < nodes->size(); ++i) {
    (*nodes)[i]->UpdateConnections(*nodes);
  }

  ConnectViaRouting(*(*nodes)[0], *(*nodes)[1]);
  ConnectViaRouting(*(*nodes)[1], *(*nodes)[2]);

  TraficGenerator trafic_generator(*nodes, 0, 15);
  auto events = std::make_unique<std::vector<std::unique_ptr<Event>>>();
  for (size_t i = 0; i < 10; ++i) {
    events->push_back(++trafic_generator);
  }
  return std::move(std::make_unique<Network>(std::move(nodes),
      std::move(events)));
}

int main() {
  // Initialize the pseudo-random generator with time as a seed.
  std::srand(std::time(0));
  auto network = std::move(CreateSimpleNetwork());

  Time simulation_duration = 500;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac

  Simulation::set_instance(std::move(network), simulation_duration,
      signal_speed_Mbps);
  Simulation::get_instance().Run();
  return 0;
}