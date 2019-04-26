//
// main.cc
//

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

#include "static_routing.h"
#include "../structure/address.h"
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

std::unique_ptr<SendEvent> CreateSendEvent(Time t, Node &sender,
    Node &reciever) {
  SimpleAddress *addr = dynamic_cast<SimpleAddress*>(
      reciever.get_addresses()[0].get());
  auto packet = std::make_unique<ProtocolPacket>(
      std::make_unique<SimpleAddress>(addr->get_address()));
  return std::move(std::make_unique<SendEvent>(t, sender, std::move(packet)));
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

  ConnectViaRouting(*nodes->operator[](0), *nodes->operator[](1));

  auto events = std::make_unique<std::vector<std::unique_ptr<Event>>>();
  events->emplace_back(CreateSendEvent(0, *nodes->operator[](0),
      *nodes->operator[](1)));

  Time simulation_duration = 20;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac
  auto simulation_parameters = std::make_unique<SimulationParameters>(
      simulation_duration, signal_speed_Mbps);

  return std::move(std::make_unique<Network>(
      std::move(nodes), std::move(events), std::move(simulation_parameters)));
}

int main() {
  auto network = std::move(CreateSimpleNetwork());

  Simulation::set_instance(std::move(network));
  Simulation::get_instance().Run();
  Simulation::get_instance().get_statistics().Print(std::cout);
  return 0;
}