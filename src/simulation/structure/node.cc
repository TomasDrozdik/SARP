//
// node.cc
//

#include "node.h"

#include "statistics.h"

namespace simulation {

Node::Node(std::vector<std::unique_ptr<Address>> addresses,
    std::unique_ptr<Routing> routing,
    std::unique_ptr<Connection> connection) :
    addresses_(std::move(addresses)),
    routing_(std::move(routing)),
    connection_(std::move(connection)) { }

void Node::UpdateConnections(const std::vector<Node *> &all_nodes) {
  active_connections_.clear();
  for (auto &node : *connection_->GetConnectedNodes(all_nodes)) {
    active_connections_.emplace_back(this, node);
  }
}

void Node::Send(std::unique_ptr<ProtocolPacket> packet) {
  auto sending_interface = routing_->Route(packet->get_destination_address());
  if (sending_interface) {
    sending_interface->Send(std::move(packet));
  } else {
    Simulation::get_instance().get_statistics().RegisterDetectedCycle();
  }
}

void Node::Recv(std::unique_ptr<ProtocolPacket> packet) {
  for (auto &addr : addresses_) {
    if (addr->operator==(packet->get_destination_address())) {
      Simulation::get_instance().get_statistics().RegisterDeliveredPacket();
      return;
    }
    Send(std::move(packet));
  }
}

const std::vector<std::unique_ptr<Address>>& Node::get_addresses() const {
  return addresses_;
}

const Connection& Node::get_connection() const {
  return *connection_;
}

}  // namespace simulation