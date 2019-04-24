//
// node.cc
//

#include "node.h"

#include "statistics.h"

namespace simulation {

Node::Node() : connection_(nullptr), routing_(nullptr){ }

void Node::UpdateConnections(
    const std::vector<std::unique_ptr<Node>> &all_nodes) {
  active_connections_.clear();
  auto nodes = *connection_->GetConnectedNodes(all_nodes);
  for (std::size_t i = 0; i < nodes.size(); ++i) {
    active_connections_.emplace_back(this, nodes[i]);
  }
}

void Node::Send(std::unique_ptr<ProtocolPacket> packet) {
  if (!IsInitialized())
    throw new UninitializedException();
  auto sending_interface = routing_->Route(packet->get_destination_address());
  if (sending_interface) {
    sending_interface->Send(std::move(packet));
  } else {
    Simulation::get_instance().get_statistics().RegisterDetectedCycle();
  }
}

void Node::Recv(std::unique_ptr<ProtocolPacket> packet) {
  if (!IsInitialized())
      throw new UninitializedException();
  for (auto &addr : addresses_) {
    if (addr->operator==(packet->get_destination_address())) {
      Simulation::get_instance().get_statistics().RegisterDeliveredPacket();
      return;
    }
    Send(std::move(packet));
  }
}

bool Node::IsInitialized() const {
  return !(addresses_.empty() || !routing_ || !connection_);
}

void Node::add_address(std::unique_ptr<Address> addr) {
  addresses_.push_back(std::move(addr));
}

void Node::set_addresses(std::vector<std::unique_ptr<Address>> addresses) {
  addresses_ = std::move(addresses);
}

void Node::set_connection(std::unique_ptr<Connection> connection) {
  connection_ = std::move(connection);
}

void Node::set_routing(std::unique_ptr<Routing> routing) {
  routing_ = std::move(routing);
}

const std::vector<std::unique_ptr<Address>>& Node::get_addresses() const {
  return addresses_;
}

const Connection& Node::get_connection() const {
  return *connection_;
}

Routing& Node::get_routing() {
  return *routing_;
}

}  // namespace simulation