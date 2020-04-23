//
// node.cc
//

#include "structure/node.h"

#include <cassert>

#include "structure/position_cube.h"
#include "structure/simulation.h"
#include "structure/statistics.h"

namespace simulation {

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
  *this = std::move(other);  // use operator==(Node &&)
}

Node &Node::operator=(Node &&node) {
  // In case of rvalue assignment unique id is just coppied.
  this->id_ = node.id_;
  this->addresses_ = node.addresses_;
  this->neighbors_ = node.neighbors_;
  this->routing_ = std::move(node.routing_);
  return *this;
}

bool Node::IsConnectedTo(const Node &node) const {
  PositionCube this_cube(position_);
  PositionCube other_node_cube(node.position_);
  uint32_t distance = PositionCube::Distance(this_cube, other_node_cube);
  // Position cube distance = 1 represents cube neighbors which see each other.
  // Calculation is based on SimulationParameters::connection_range
  return distance <= 1;
}

void Node::UpdateNeighbors(std::set<Node *> neighbors) {
  neighbors_ = neighbors;
  routing_->UpdateNeighbors();
}

void Node::Send(std::unique_ptr<Packet> packet) {
  assert(IsInitialized());
  assert(packet != nullptr);

  Node *to_node = routing_->Route(*packet);
  if (to_node) {
    if (neighbors_.find(to_node) == neighbors_.end() ||
        !IsConnectedTo(*to_node)) {
      Statistics::RegisterRoutingResultNotNeighbor();
      return;
    }
    Time delivery_duration = SimulationParameters::DeliveryDuration(
        *this, *to_node, packet->get_size());
    Simulation::get_instance().ScheduleEvent(
        std::make_unique<RecvEvent>(delivery_duration, TimeType::RELATIVE,
                                    *this, *to_node, std::move(packet)));
  } else {
    // Routing did not find a route for the packet so just report it.
    if (packet->IsRoutingUpdate()) {
      Statistics::RegisterRoutingOverheadLoss();
    } else {
      Statistics::RegisterDataPacketLoss();
    }
  }
}

void Node::Recv(std::unique_ptr<Packet> packet, Node *from_node) {
  assert(IsInitialized());
  if (packet->IsTTLExpired()) {
    return;
  }
  // Process the packet on routing. If false stop processing.
  if (packet->IsRoutingUpdate()) {
    routing_->Process(*packet, from_node);
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
