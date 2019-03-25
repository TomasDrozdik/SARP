//
// node.cc
//

#include "node.h"

#include <cassert>

#include "statistics.h"

namespace simulation {

Node::Node(std::set<std::unique_ptr<Address>> &addresses,
    Routing &routing, Connection &connection, const int process_power = 5) :
    addresses_(addresses),
    routing_(routing),
    connection_(connection),
    process_power_(process_power) { }

void Node::UpdateConnections(const std::set<Node const * const> &all_nodes) {
  active_connections_.clear();
  for (auto &node : *connection_.GetConnectedNodes(all_nodes)) {
    active_connections_.emplace(this, node);
  }
}

void Node::Send(std::unique_ptr<ProtocolPacket> &&packet) {
  auto sending_interface = routing_.Route(packet->get_destination_address());
  if (sending_interface) {
    sending_interface->Send(std::move(packet));
  } else {
    Simulation::get_instance().get_statistics().RegisterDetectedCycle();
  }
}

void Node::Recv(std::unique_ptr<ProtocolPacket> &&packet) {
  network_stack_.push(std::move(packet));
}

void Node::Process() {
  if (network_stack_.empty()) {
    Simulation::get_instance().get_statistics().RegisterUndeliveredPacket();
    return;
  }

  auto packet = std::move(network_stack_.top());
  for (auto &addr : addresses_) {
    if (addr->operator==(packet->get_destination_address())) {
      Simulation::get_instance().get_statistics().RegisterDeliveredPacket();
      return;
    }
  }
  Send(std::move(packet));
}

const int Node::get_processing_power() const {
  return process_power_;
}

const std::set<std::unique_ptr<Address>>& Node::get_addresses() const {
  return addresses_;
}

const Connection& Node::get_connection() const {
  return connection_;
}

}  // namespace simulation