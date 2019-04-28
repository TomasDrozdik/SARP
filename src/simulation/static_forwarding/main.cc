//
// main.cc
//
#include <map>

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
  SimpleAddress &sender_addr = *dynamic_cast<SimpleAddress*>(
      sender.get_addresses()[0].get());
  SimpleAddress &reciever_addr = *dynamic_cast<SimpleAddress*>(
      reciever.get_addresses()[0].get());
  auto packet = std::make_unique<ProtocolPacket>(
      std::make_unique<SimpleAddress>(sender_addr),
      std::make_unique<SimpleAddress>(reciever_addr));
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

  nodes->push_back(CreateNode(std::make_unique<SimpleAddress>(2),
      Position(2, 2, 2)));

  for (std::size_t i = 0; i < nodes->size(); ++i) {
    (*nodes)[i]->UpdateConnections(*nodes);
  }

  ConnectViaRouting(*nodes->operator[](0), *nodes->operator[](1));
  ConnectViaRouting(*nodes->operator[](1), *nodes->operator[](2));

  auto events = std::make_unique<std::vector<std::unique_ptr<Event>>>();
  events->emplace_back(CreateSendEvent(0, *nodes->operator[](0),
      *nodes->operator[](2)));

  events->emplace_back(CreateSendEvent(5, *nodes->operator[](1),
      *nodes->operator[](2)));

  return std::move(std::make_unique<Network>(
      std::move(nodes), std::move(events)));
}

int main() {
  auto network = std::move(CreateSimpleNetwork());

  Time simulation_duration = 500;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac

  Simulation::set_instance(std::move(network), simulation_duration,
      signal_speed_Mbps);
  Simulation::get_instance().Run();
  return 0;
}