//
// node.cc
//

#include "node.h"

#include <cassert>

namespace simulation {

Node::Node(const Position position, Address &addr, Routing &routing,
    int connection_range = 100, int processing_power = 5) :
    position_(position),
    addr_(addr),
    routing_(routing),
    connection_range_(connection_range),
    process_power_(processing_power) { }

void Node::UpdateConnections(std::set<const Node&> &all_nodes) {
  this->active_connections_.clear();
  for (auto &node : all_nodes) {
    int distance = Position::Distance(this->position_, node.position_);
    // Count if the distance is lower than range of the connection
    if (this->connection_range_ <= distance) {
      auto ret = active_connections_.emplace(this, node);

      // Assert that emplace was successful
      assert(ret.second);
    }
  }
}

void Node::Send(std::unique_ptr<ProtocolPacket> &&packet) {
  routing_.Route(packet->get_destination_address())
      .Send(std::move(packet));
}

void Node::Recv(std::unique_ptr<ProtocolPacket> &&packet) {
  network_stack_.push(std::move(packet));
}

void Node::Process() {
  if (network_stack_.empty()) {
    // TODO: statistics add unsuccessfull delivery
    return;
  }

  auto packet = std::move(network_stack_.top());
  if (addr_.operator==(packet->get_destination_address())) {
    // TODO: statistics add successful delivery
  } else {
    // TODO: add cycle detection option
    Send(std::move(packet));
  }
}

const Address& Node::get_address() const {
  return addr_;
}

const int Node::get_processing_power() const {
  return process_power_;
}

}  // namespace simulation