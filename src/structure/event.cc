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

Event::~Event() {}

bool Event::operator<(const Event &other) const {
  if (time_ == other.time_) {
    // Since priority is not inverted i.e. the bigger the number the bigger
    // priority the event has we have to invert it here.
    return get_priority() > other.get_priority();
  }
  return time_ < other.time_;
}

InitNetworkEvent::InitNetworkEvent(const Time time, TimeType time_type,
                                   Network &network)
    : Event(time, time_type), network_(network) {}

void InitNetworkEvent::Execute(Env &env) { network_.Init(env); }

std::ostream &InitNetworkEvent::Print(std::ostream &os) const {
  return os << time_ << ":init_network:update_neighbors+init_routing\n";
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
  if (reciever_.IsConnectedTo(sender_, env.parameters.connection_range)) {
    reciever_.Recv(env, std::move(packet_), &sender_);
  }
}

std::ostream &RecvEvent::Print(std::ostream &os) const {
  assert(packet_ != nullptr);
  return os << time_ << ":recv:" << reciever_ << " --" << *packet_ << "--> ["
            << packet_->get_destination_address() << "]\n";
}

MoveEvent::MoveEvent(const Time time, TimeType time_type, Node &node,
                     Network &network, Position new_position)
    : Event(time, time_type),
      node_(node),
      network_(network),
      new_position_(new_position) {}

void MoveEvent::Execute(Env &env) {
  env.stats.RegisterMoveEvent();

#ifdef DEBUG  // Check whether the node doesn't cross boundaries.
  Position pos = new_position_;
  Position min = env.parameters.position_min;
  Position max = env.parameters.position_max;
  assert(pos.x >= min.x && pos.y >= min.y && pos.z >= min.z);
  assert(pos.x <= max.x && pos.y <= max.y && pos.z <= max.z);
#endif  // DEBUG

  PositionCube old_cube(node_.get_position(), env.parameters.connection_range);
  PositionCube new_cube(new_position_, env.parameters.connection_range);
  if (old_cube != new_cube) {
    network_.UpdateNodePosition(node_, new_cube, env.parameters.position_min,
                                env.parameters.position_max,
                                env.parameters.connection_range);
  }
  node_.set_position(new_position_);
}

std::ostream &MoveEvent::Print(std::ostream &os) const {
  return os << time_ << ":move:" << node_ << " --> " << new_position_ << '\n';
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

}  // namespace simulation
