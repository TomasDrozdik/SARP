//
// events.cc
//

#include "event.h"

#include "protocol_packet.h"
#include "simulation.h"
#include "interface.h"

namespace simulation {

std::ostream &operator<<(std::ostream os, const Event &event) {
  return event.Print(os);
}

Event::Event(Time time, bool is_absolute_time) : time_(time),
    is_absolute_time_(is_absolute_time) { }

Event::~Event() { }

bool Event::operator<(const Event &other) const {
  return time_ < other.time_;
}

SendEvent::SendEvent(const Time time, Node &sender,
    std::unique_ptr<ProtocolPacket> packet) :
        Event(time), sender_(sender), packet_(std::move(packet)) { }

void SendEvent::Execute() {
  sender_.Send(std::move(packet_));
}

std::ostream &SendEvent::Print(std::ostream &os) const {
  return os << time_ << ":send:" << sender_ <<
      " --" << *packet_ << "--> [" <<
      *packet_->get_destination_address() << "]\n";
}

RecvEvent::RecvEvent(const Time time, Interface &reciever,
    std::unique_ptr<ProtocolPacket> packet) :
        Event(time), reciever_(reciever), packet_(std::move(packet)) { }

void RecvEvent::Execute() {
  reciever_.Recv(std::move(packet_));
}

std::ostream &RecvEvent::Print(std::ostream &os) const {
  return os << time_ << ":recv:" << reciever_.get_node() <<
      " --" << *packet_ << "--> [" <<
      *packet_->get_destination_address() << "]\n";
}

MoveEvent::MoveEvent(const Time time, Node &node, Position new_position) :
    Event(time), node_(node), new_position_(new_position) { }

void MoveEvent::Execute() {
  node_.get_connection().position = new_position_;
}

std::ostream &MoveEvent::Print(std::ostream &os) const {
  return os << time_ << ":move:" << node_ << " --> [" << new_position_ << ")\n";
}

UpdateConnectionsEvent::UpdateConnectionsEvent(const Time time,
    Network &network) : Event(time), network_(network) { }

void UpdateConnectionsEvent::Execute() {
  network_.UpdateConnections();
}

std::ostream &UpdateConnectionsEvent::Print(std::ostream &os) const {
  return os << time_ << ":update_connections:\n";
}

}  // namespace simulation
