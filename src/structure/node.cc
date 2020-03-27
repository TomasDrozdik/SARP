//
// node.cc
//

#include "structure/node.h"

#include <cassert>

#include "structure/position_cube.h"
#include "structure/simulation.h"
#include "structure/statistics.h"

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
  char delim = 0;
  for (const auto &addr : node.addresses_) {
    os << delim << addr;
    delim = ',';
  }
  os << '>';
  return os;
}

Node::Node() {
  // Assign unique id.
  assert(Node::id_counter_ != std::numeric_limits<size_t>::max());
  id_ = Node::id_counter_++;
}

Node::Node(Node &&other) {
  *this = std::move(other);  // use operator(Node &&)
}

Node &Node::operator=(Node &&node) {
  // In case of rvalue assignment unique id is just coppied.
  this->id_ = node.id_;
  this->addresses_ = std::move(node.addresses_);
  this->active_interfaces_ = std::move(node.active_interfaces_);
  this->routing_ = std::move(node.routing_);
  return *this;
}

bool Node::IsConnectedTo(const Node &node) const {
  PositionCube this_cube(position_);
  PositionCube other_node_cube(node.position_);
  uint32_t distance = PositionCube::Distance(this_cube, other_node_cube);
  // Position cube distance = 1 represents cube neighbors which see each other.
  // Calculation is based on SimulationParameters::connection_range_
  return distance <= 1;
}

void Node::Send(std::unique_ptr<ProtocolPacket> packet) {
  assert(IsInitialized());
  assert(packet != nullptr);

  Interface *sending_interface = routing_->Route(*packet);
  assert(packet != nullptr);
  if (sending_interface) {
    sending_interface->Send(std::move(packet));
  } else {
    // Routing did not find a route for the packet so just report it.
    if (packet->IsRoutingUpdate()) {
      Statistics::RegisterRoutingOverheadLoss();
    } else {
      Statistics::RegisterDataPacketLoss();
    }
  }
}

void Node::Recv(std::unique_ptr<ProtocolPacket> packet,
                Interface *processing_interface) {
  assert(IsInitialized());
  // Process the packet on routing. If false stop processing.
  if (packet->IsRoutingUpdate()) {
    routing_->Process(*packet, processing_interface);
    return;
  }
  // Check for match in destination_address on packet.
  for (const auto &addr : addresses_) {
    if (addr == packet->get_destination_address()) {
      Statistics::RegisterDeliveredPacket();
      return;
    }
  }
  Statistics::RegisterHop();
  // TODO: may use some constant as matter of processing time on a node.
  Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
      1, TimeType::RELATIVE, *this, std::move(packet)));
}

}  // namespace simulation
