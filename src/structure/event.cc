//
// events.cc
//

#include "structure/event.h"

#include <cassert>

#include "structure/interface.h"
#include "structure/protocol_packet.h"
#include "structure/simulation.h"
#include "structure/statistics.h"

namespace simulation {

std::ostream &operator<<(std::ostream os, const Event &event) {
  return event.Print(os);
}

Event::Event(Time time, TimeType time_type)
    : time_(time), time_type_(time_type) {}

Event::~Event() {}

bool Event::operator<(const Event &other) const { return time_ < other.time_; }

SendEvent::SendEvent(const Time time, TimeType time_type, Node &sender,
                     std::unique_ptr<ProtocolPacket> packet)
    : Event(time, time_type), sender_(sender), packet_(std::move(packet)) {}

SendEvent::SendEvent(const Time time, TimeType time_type, Node &sender,
                     Node &destination, uint32_t size)
    : Event(time, time_type),
      sender_(sender),
      destination_(&destination),
      size_(size) {}

void SendEvent::Execute() {
  Statistics::RegisterSendEvent();
  if (packet_) {
    sender_.Send(std::move(packet_));
  } else {
    // Create packet here because we want to have actual addresses of nodes.
    // These data packets are planned ahead of simulation.
    auto packet = std::make_unique<ProtocolPacket>(sender_.get_address(),
                                                   destination_->get_address(),
                                                   PacketType::DATA, size_);
    sender_.Send(std::move(packet));
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

RecvEvent::RecvEvent(const Time time, TimeType time_type, Interface &reciever,
                     std::unique_ptr<ProtocolPacket> packet)
    : Event(time, time_type), reciever_(reciever), packet_(std::move(packet)) {
  assert(packet_ != nullptr);
}

void RecvEvent::Execute() {
  Statistics::RegisterRecvEvent();
  assert(packet_ != nullptr);
  reciever_.Recv(std::move(packet_));
}

std::ostream &RecvEvent::Print(std::ostream &os) const {
  assert(packet_ != nullptr);
  return os << time_ << ":recv:" << reciever_.get_node() << " --" << *packet_
            << "--> [" << packet_->get_destination_address() << "]\n";
}

MoveEvent::MoveEvent(const Time time, TimeType time_type, Node &node,
                     Position new_position)
    : Event(time, time_type), node_(node), new_position_(new_position) {}

void MoveEvent::Execute() {
  Statistics::RegisterMoveEvent();
  node_.set_position(new_position_);
}

std::ostream &MoveEvent::Print(std::ostream &os) const {
  return os << time_ << ":move:" << node_ << " --> [" << new_position_ << "]\n";
}

UpdateInterfacesEvent::UpdateInterfacesEvent(const Time time,
                                             TimeType time_type,
                                             Network &network)
    : Event(time, time_type), network_(network) {}

void UpdateInterfacesEvent::Execute() {
  Statistics::RegisterUpdateInterfacesEvent();
  network_.UpdateInterfaces();
}

std::ostream &UpdateInterfacesEvent::Print(std::ostream &os) const {
  return os << time_ << ":connections_update:\n";
}

UpdateRoutingInterfacesEvent::UpdateRoutingInterfacesEvent(const Time time,
                                                           TimeType time_type,
                                                           Routing &routing)
    : Event(time, time_type), routing_(routing) {}

void UpdateRoutingInterfacesEvent::Execute() {
  Statistics::RegisterUpdateRoutingInterfacesEvent();
  routing_.UpdateInterfaces();
}

std::ostream &UpdateRoutingInterfacesEvent::Print(std::ostream &os) const {
  return os << time_ << ":routing_interfaces_update:" << routing_ << '\n';
}

UpdateRoutingEvent::UpdateRoutingEvent(const Time time, TimeType time_type,
                                       Routing &routing)
    : Event(time, time_type), routing_(routing) {}

void UpdateRoutingEvent::Execute() {
  Statistics::RegisterUpdateRoutingEvent();
  routing_.CheckPeriodicUpdate();
}

std::ostream &UpdateRoutingEvent::Print(std::ostream &os) const {
  return os << time_ << ":routing_update:" << routing_ << '\n';
}

}  // namespace simulation
