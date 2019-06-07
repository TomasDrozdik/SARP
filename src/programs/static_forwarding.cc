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

#include "../static_forwarding/static_routing.h"
#include "../network_generator/position_generator.h"
#include "../network_generator/trafic_generator.h"
#include "../network_generator/address_iterator.h"
#include "../structure/event.h"
#include "../structure/network.h"
#include "../structure/node.h"
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
  AddressIterator<SimpleAddress> addr_iterator;
  std::vector<std::unique_ptr<Node>> nodes;
  nodes.push_back(CreateNode(++addr_iterator, Position(0, 0, 0)));
  nodes.push_back(CreateNode(++addr_iterator, Position(1, 1, 1)));

  auto network = std::make_unique<Network>(std::move(nodes));

  ConnectViaRouting(*network->get_nodes()[0], *network->get_nodes()[1]);

  return std::move(network);
}

int main() {
  auto network = CreateSimpleNetwork();

  for (const auto &node : network->get_nodes()) {
    node->Print(); std::cout << std::endl;
    for (auto &iface : node->get_active_connections()) {
      iface.Print();
    }
    std::cout << std::endl;
  }

  Time trafic_start = 0;
  Time trafic_end = 400;
  TraficGenerator trafic_generator(network->get_nodes(), trafic_start,
      trafic_end);

  auto events = std::vector<std::unique_ptr<Event>>();
  for (size_t i = 0; i < 10; ++i) {
    events.push_back(++trafic_generator);
  }

  Time simulation_duration = 500;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac

  Simulation::set_instance(std::move(network), simulation_duration,
      signal_speed_Mbps);
  Simulation::get_instance().Run(std::move(events));
  return 0;
}
