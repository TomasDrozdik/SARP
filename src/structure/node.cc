//
// node.cc
//

#include "structure/node.h"

#include <cassert>

#include "structure/simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Node &node) {
  if (node.addresses_.empty()) {
    return os << '<' << node.id_ << ":NONE>";
  }

  os << '<' << node.id_ << ':';
  char delim = 0;
  for (const auto &addr : node.addresses_) {
    os << delim << addr;
    delim = ',';
  }
  os << '>';
  return os;
}

Node &Node::operator=(Node &&node) {
  // In case of rvalue assignment unique id is just coppied.
  this->id_ = node.id_;
  this->addresses_ = node.addresses_;
  this->neighbors_ = node.neighbors_;
  this->routing_ = std::move(node.routing_);
  return *this;
}

bool Node::IsConnectedTo(const Node &node, uint32_t connection_range) const {
  uint32_t distance = Position::Distance(position_, node.position_);
  return distance <= connection_range;
}

void Node::UpdateNeighbors(Env &env, std::set<Node *> new_neighbors) {
  routing_->UpdateNeighbors(env, new_neighbors);
  neighbors_ = new_neighbors;
}

static Time DeliveryDuration(const Node &n1, const Node &n2) {
  uint32_t distance = Position::Distance(n1.get_position(), n2.get_position());
  Time t = distance / 10;
  return (t < 1) ? 1 : t;
}

void Node::Send(Env &env, std::unique_ptr<Packet> packet) {
  assert(IsInitialized());
  assert(packet != nullptr);

  Node *to_node = routing_->Route(env, *packet);
  if (to_node) {
    if (!IsConnectedTo(*to_node, env.parameters.get_connection_range())) {
      env.stats.RegisterRoutingResultNotNeighbor();
      return;
    }
    Time delivery_duration = DeliveryDuration(*this, *to_node);
    env.simulation.ScheduleEvent(
        std::make_unique<RecvEvent>(delivery_duration, TimeType::RELATIVE,
                                    *this, *to_node, std::move(packet)));
  } else {
    // Routing did not find a route for the packet so just report it.
    if (packet->IsRoutingUpdate()) {
      env.stats.RegisterRoutingOverheadLoss();
    } else {
      env.stats.RegisterDataPacketLoss();
    }
  }
}

void Node::Recv(Env &env, std::unique_ptr<Packet> packet, Node *from_node) {
  assert(IsInitialized());
  if (packet->IsTTLExpired(env.parameters.get_ttl_limit())) {
    env.stats.RegisterTTLExpire();
    return;
  }
  // Process the packet on routing. If false stop processing.
  if (packet->IsRoutingUpdate()) {
    routing_->Process(env, *packet, from_node);
    return;
  }
  // Check for match in destination_address on packet.
  for (const auto &addr : addresses_) {
    if (addr == packet->get_destination_address()) {
      env.stats.RegisterDeliveredPacket();
      return;
    }
  }
  env.stats.RegisterHop();
  // TODO: may use some constant as matter of processing time on a node.
  env.simulation.ScheduleEvent(std::make_unique<SendEvent>(
      1, TimeType::RELATIVE, *this, std::move(packet)));
}

void Node::AddAddress(Address addr) {
  assert(IsInitialized());
  auto [it, success] = addresses_.insert(addr);
  assert(success);  // TODO maybe remove
  latest_address_ = it;
  routing_->UpdateAddresses();
}

void Node::InitializeAddresses(Node::AddressContainerType addresses) {
  assert(IsInitialized());
  addresses_ = addresses;
  latest_address_ = addresses_.begin();
  routing_->UpdateAddresses();
}

static double NormalizeDouble(const double &d) {
  if (d > 0 && d < 1) {
    return 1;
  } else if (d > -1 && d < 0) {
    return -1;
  }
  return d;
}

void Node::Move(Time time_diff) {
  if (!has_mobility_plan()) {
    return;
  }
  auto &plan = mobility_.second;
  if (position_ == plan.destination) {
    if (plan.pause == 0) {
      // We have reached the destination and waited for selected pause.
      // Note that the plan is finished.
      mobility_.first = false;
    } else {
      plan.pause -= time_diff;
    }
  } else {
    // Move in given direction with given speed in time_diff.
    double vector_x = plan.destination.x - position_.x;
    double vector_y = plan.destination.y - position_.y;
    double vector_z = plan.destination.z - position_.z;
    double distance = std::sqrt(vector_x * vector_x + vector_y * vector_y +
                                vector_z * vector_z);
    double seconds = time_diff / 1000;
    double travel_distance = plan.speed * seconds;
    if (NormalizeDouble(travel_distance) > distance) {
      position_ = plan.destination;
    } else {
      double scale = travel_distance / distance;
      vector_x *= scale;
      vector_y *= scale;
      vector_z *= scale;
      position_.x += NormalizeDouble(vector_x);
      position_.y += NormalizeDouble(vector_y);
      position_.z += NormalizeDouble(vector_z);
    }
  }
}

}  // namespace simulation
