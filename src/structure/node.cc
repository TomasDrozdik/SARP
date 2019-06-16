//
// node.cc
//

#include "node.h"

#include <cassert>

#include "simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Node &node) {
  os << '<';
  for (std::size_t i = 0; i < node.addresses_.size(); ++i) {
    if (i != node.addresses_.size() - 1) {
      node.addresses_[i]->Print(os);
      os << ',';
    } else {
      node.addresses_[i]->Print(os);
      os << '>';
    }
  }
  return os;
}

Node::Node() : connection_(nullptr), routing_(nullptr) { }

void Node::Send(std::unique_ptr<ProtocolPacket> packet) {
  assert(IsInitialized());
  auto sending_interface = routing_->Route(*packet);
  if (sending_interface) {
    sending_interface->Send(std::move(packet));
  } else {
    Simulation::get_instance().get_statistics().RegisterUndeliveredPacket();
  }
}

void Node::Recv(std::unique_ptr<ProtocolPacket> packet,
    Interface *processing_interface) {
  assert(IsInitialized());
  // Process the packet on routing. If false stop processing.
  if (packet->IsRoutingUpdate() &&
       !routing_->Process(*packet, processing_interface)) {
    return;
  }
  // Check for match in destination_address on packet.
  for (auto &addr : addresses_) {
    if (*addr == *packet->get_destination_address()) {
      Simulation::get_instance().get_statistics().RegisterDeliveredPacket();
      return;
    }
  }
  Simulation::get_instance().get_statistics().RegisterHop();
  // Make an instantanious send without a SendEvent.
  // TODO: may use some constant as matter of processing time on a node.
  //       and so schedule new SendEvent.
  Send(std::move(packet));
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

std::vector<std::shared_ptr<Interface>>& Node::get_active_connections() {
  return active_connections_;
}

const std::unique_ptr<Address>& Node::get_address() const {
  return addresses_[0];
}

const std::vector<std::unique_ptr<Address>>& Node::get_addresses() const {
  return addresses_;
}

const Connection& Node::get_connection() const {
  return *connection_;
}

Connection& Node::get_connection() {
  return *connection_;
}

Routing& Node::get_routing() {
  return *routing_;
}

}  // namespace simulation
