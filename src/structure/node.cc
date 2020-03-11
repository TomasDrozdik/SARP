//
// node.cc
//

#include "structure/node.h"

#include <cassert>

#include "structure/simulation.h"

namespace simulation {

std::size_t UniquePtrInterfaceHash::operator()(
      const std::unique_ptr<simulation::Interface> &i) const {
  return std::hash<const simulation::Node *>()(&i->get_node()) ^
      (std::hash<const simulation::Node *>()(&i->get_other_end_node()) << 1);
}

bool UniquePtrInterfaceEqualTo::operator()(
    const std::unique_ptr<simulation::Interface> &i1,
    const std::unique_ptr<simulation::Interface> &i2) const {
  return *i1 == *i2;
}

std::ostream &operator<<(std::ostream &os, const Node &node) {
  if (node.addresses_.empty()) {
    return os << "<NONE>";
  }

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

Node::Node() {
  // Assign unique id.
  assert(Node::id_counter_ != std::numeric_limits<size_t>::max());
  id_ = Node::id_counter_++;
}

Node::Node(Node &&other) {
  *this = std::move(other);   // use operator(Node &&)
}

Node &Node::operator=(Node &&node) {
  // In case of rvalue assignment unique id is just coppied.
  this->id_ = node.id_;
  this->addresses_ = std::move(node.addresses_);
  this->active_interfaces_ = std::move(node.active_interfaces_);
  this->connection_ = std::move(node.connection_);
  this->routing_ = std::move(node.routing_);
  return *this;
}

void Node::Send(std::unique_ptr<ProtocolPacket> packet) {
  assert(IsInitialized());
  Interface *sending_interface = routing_->Route(*packet);
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
  if (packet->is_routing_update() &&
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
  // TODO: may use some constant as matter of processing time on a node.
  Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, false, *this, std::move(packet)));
}

}  // namespace simulation
