//
// events.cc
//

#include "structure/event.h"

#include <cassert>

#include "structure/simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream os, const Event &event) {
  return event.Print(os);
}

Event::Event(Time time, TimeType time_type)
    : time_(time), time_type_(time_type) {}

bool Event::operator<(const Event &other) const {
  if (time_ == other.time_) {
    // Since priority is not inverted i.e. the bigger the number the bigger
    // priority the event has we have to invert it here.
    return get_priority() > other.get_priority();
  }
  return time_ < other.time_;
}

SendEvent::SendEvent(const Time time, TimeType time_type, Node &sender,
                     std::unique_ptr<Packet> packet)
    : Event(time, time_type), sender_(sender), packet_(std::move(packet)) {}

SendEvent::SendEvent(const Time time, TimeType time_type, Node &sender,
                     Node &destination, uint32_t size)
    : Event(time, time_type),
      sender_(sender),
      destination_(&destination),
      size_(size) {}

void SendEvent::Execute(Env &env) {
  env.stats.RegisterSendEvent();
  if (packet_) {
    sender_.Send(env, std::move(packet_));
  } else {
    // Create packet here because we want to have actual addresses of nodes.
    // These data packets are planned ahead of simulation.
    auto packet = std::make_unique<Packet>(sender_.get_address(),
                                           destination_->get_address(),
                                           PacketType::DATA, size_);
    sender_.Send(env, std::move(packet));
  }
}

std::ostream &SendEvent::Print(std::ostream &os) const {
  if (packet_) {
    return os << time_ << ":send:" << sender_ << " --" << *packet_ << "--> ["
              << packet_->get_destination_address() << "]\n";
  } else {
    return os << time_ << ":send:" << sender_ << " --{data_" << size_
              << "}--> [" << destination_->get_address() << "]\n";
  }
}

RecvEvent::RecvEvent(const Time time, TimeType time_type, Node &sender,
                     Node &reciever, std::unique_ptr<Packet> packet)
    : Event(time, time_type),
      sender_(sender),
      reciever_(reciever),
      packet_(std::move(packet)) {
  assert(packet_ != nullptr);
}

void RecvEvent::Execute(Env &env) {
  env.stats.RegisterRecvEvent();
  assert(packet_ != nullptr);
  // WARNING: Here is a simplification, RecvEvent is only successful if both
  // sender and reciever are connected at the time of the recieve.
  if (reciever_.IsConnectedTo(sender_, env.parameters.get_general().connection_range)) {
    reciever_.Recv(env, std::move(packet_), &sender_);
  }
}

std::ostream &RecvEvent::Print(std::ostream &os) const {
  assert(packet_ != nullptr);
  return os << time_ << ":recv:" << reciever_ << " --" << *packet_ << "--> ["
            << packet_->get_destination_address() << "]\n";
}

TrafficEvent::TrafficEvent(Time time, TimeType time_type, Network &network,
    NodeID from, NodeID to)
  : Event(time, time_type), network_(network), from_(from), to_(to) {}

void TrafficEvent::Execute(Env &env) {
  auto sender = network_.get_node(from_);
  auto receiver = network_.get_node(to_);
  if (sender == nullptr || receiver == nullptr) {
    // TODO: register failed specific traffic.
    return;
  }
  uint32_t packet_size = 1;
  auto send_event = std::make_unique<SendEvent>(
      0, TimeType::RELATIVE, *sender, *receiver, packet_size);
  env.simulation.ScheduleEvent(std::move(send_event));
}

std::ostream &TrafficEvent::Print(std::ostream &os) const {
  return os << time_ << ":traffic: <" << from_ << "> -- <" << to_ << ">\n";
}

RandomTrafficEvent::RandomTrafficEvent(Time time, TimeType time_type,
    Network &network)
  : Event(time, time_type), network_(network) {}

void RandomTrafficEvent::Execute(Env &env) {
  auto &nodes = network_.get_nodes();
  if (nodes.size() < 2) {
    return;
  }
  std::size_t r1 = std::rand() % nodes.size();
  std::size_t r2 = std::rand() % nodes.size();
  if (r1 == r2) {  // Avoid reflexive traffic.
    r2 = (r2 + 1) % nodes.size();
  }
  Node &sender = *nodes[r1];
  Node &receiver = *nodes[r2];
  uint32_t packet_size = 1;
  auto send_event = std::make_unique<SendEvent>(
      0, TimeType::RELATIVE, sender, receiver, packet_size);
  env.simulation.ScheduleEvent(std::move(send_event));
}

std::ostream &RandomTrafficEvent::Print(std::ostream &os) const {
  return os << time_ << ":random_traffic:\n";
}

MoveEvent::MoveEvent(Time time, TimeType time_type, Network &network,
    Node &node, std::unique_ptr<PositionGenerator> directions)
    : Event(time, time_type), network_(network), node_(node),
      directions_(std::move(directions)) {}

void MoveEvent::Execute(Env &env) {
  assert(env.parameters.has_movement());
  env.stats.RegisterMoveEvent();
  if (env.simulation.get_current_time() >= env.parameters.get_movement().end) {
    return;
  }
  if (node_.has_mobility_plan() == false) {
    if (AssignNewPlan(env.parameters) == false) {
      return;  // There is no new plan i.e. exit.
    }
  }
  const Position old_position = node_.get_position();
  const Time period = env.parameters.get_movement().step_period;
  node_.Move(period);
  network_.UpdateNodePosition(env.parameters, node_, old_position);
  // Since the movement hasn't stopped plan next event.
  env.simulation.ScheduleEvent(std::make_unique<MoveEvent>(
        period, TimeType::RELATIVE, network_, node_, std::move(directions_)));
}

static double GetRandomDouble(double min, double max) {
  assert(min < max);
  double f = (double)std::rand() / RAND_MAX;
  return min + f * (max - min);
}

bool MoveEvent::AssignNewPlan(const Parameters &parameters) {
  if (directions_ == nullptr) {
    directions_ = parameters.get_movement().directions->Clone();
    if (directions_ == nullptr) {
      return false;
    }
  }
  const auto [destination, success] = directions_->Next();
  if (!success) {
    return false;
  }
  const auto &speed_range = parameters.get_movement().speed_range;
  double speed = (speed_range.second <= speed_range.first)
                     ? speed_range.second
                     : GetRandomDouble(speed_range.first, speed_range.second);
  const auto &pause_range = parameters.get_movement().pause_range;
  Time pause = (pause_range.second <= pause_range.first)
                   ? pause_range.second
                   : GetRandomDouble(pause_range.first, pause_range.second);
  node_.set_mobility_plan(
      {.destination = destination, .speed = speed, .pause = pause});
  return true;
}

std::ostream &MoveEvent::Print(std::ostream &os) const {
  return os << time_ << ":move:" << node_ << " at " << node_.get_position()
            << '\n';
}

UpdateNeighborsEvent::UpdateNeighborsEvent(const Time time, TimeType time_type,
                                           Network &network)
    : Event(time, time_type), network_(network) {}

void UpdateNeighborsEvent::Execute(Env &env) {
  env.stats.RegisterUpdateNeighborsEvent();
  network_.UpdateNeighbors(env);
}

std::ostream &UpdateNeighborsEvent::Print(std::ostream &os) const {
  return os << time_ << ":update_neighbors:\n";
}

UpdateRoutingEvent::UpdateRoutingEvent(const Time time, TimeType time_type,
                                       Routing &routing)
    : Event(time, time_type), routing_(routing) {}

void UpdateRoutingEvent::Execute(Env &env) {
  env.stats.RegisterUpdateRoutingEvent();
  routing_.CheckPeriodicUpdate(env);
}

std::ostream &UpdateRoutingEvent::Print(std::ostream &os) const {
  return os << time_ << ":routing_update:" << routing_ << '\n';
}

RequestUpdateEvent::RequestUpdateEvent(const Time time, TimeType time_type, Node *node, Node *neighbor)
  : Event(time, time_type), node_(node), neighbor_(neighbor) {}

void RequestUpdateEvent::Execute(Env &env) {
  neighbor_->get_routing().SendUpdate(env, node_);
}

std::ostream &RequestUpdateEvent::Print(std::ostream &os) const {
  return os << time_ << ":request_update:" << *node_ << " <-- " << *neighbor_ << '\n'; 
}

BootEvent::BootEvent(const Time time, TimeType time_type, Network &network,
    std::unique_ptr<Node> node, std::unique_ptr<PositionGenerator> directions) 
  : Event(time, time_type), network_(network), node_(std::move(node)), 
    directions_(std::move(directions)) {}

void BootEvent::Execute(Env &env) {
  auto &node = network_.AddNode(env.parameters, std::move(node_));
  node.get_routing().Init(env);
  if (env.parameters.has_movement()) {
    // Schedule a first move event which does pick information form env on how
    // to move the node.
    // Leave one routing period for synchronization.
    env.simulation.ScheduleEvent(std::make_unique<MoveEvent>(
        env.parameters.get_general().routing_update_period,
        TimeType::RELATIVE, network_, node, std::move(directions_)));
  }
}

std::ostream &BootEvent::Print(std::ostream &os) const {
  return os << time_ << ":boot_node:" << *node_ << '\n'; 
}

ReaddressEvent::ReaddressEvent(Time time, TimeType time_type, Network &network,
    bool only_empty)
    : Event(time, time_type), network_(network), only_empty_(only_empty) {}

void ReaddressEvent::Execute(Env &env) {
  for (auto &node : network_.get_nodes()) {
    if (only_empty_ && node->get_addresses().empty() == false) {
      continue;
    }
    Address last_address = node->get_address();
    const auto [new_address, success] = node->get_routing().SelectAddress(env);
    if (!success) {
      continue;
    }
    // Keep last address and new one.
    node->InitializeAddresses(Node::AddressContainerType(
          {new_address, last_address}));
  }
}

std::ostream &ReaddressEvent::Print(std::ostream &os) const {
  return os << time_ << ":readdress_event:" << '\n'; 
}

}  // namespace simulation
