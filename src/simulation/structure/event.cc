//
// events.cc
//

#include "event.h"

#include <cstdio>

#include "protocol_packet.h"
#include "simulation.h"
#include "interface.h"

namespace simulation {

Event::Event(Time time, bool is_absolute_time) : time_(time),
    is_absolute_time_(is_absolute_time) { }

Event::~Event() { }

bool Event::operator<(const Event &other) const {
  return time_ < other.time_;
}

SendEvent::SendEvent(const Time time, const Node &sender,
    std::unique_ptr<ProtocolPacket> packet) :
        Event(time), sender_(sender), packet_(std::move(packet)) { }

void SendEvent::Execute() {
  sender_.Send(std::move(packet_));
}

void SendEvent::Print() {
  std::printf("%zu:send:", this->time_);
  sender_.Print();
  std::printf(" --> [%s]\n", static_cast<std::string>(
      packet_->get_destination_address()).c_str());
}

RecvEvent::RecvEvent(const Time time, const Node &reciever,
    std::unique_ptr<ProtocolPacket> packet) :
        Event(time), reciever_(reciever), packet_(std::move(packet)) { }

void RecvEvent::Execute() {
  reciever_.Recv(std::move(packet_));
}

void RecvEvent::Print() {
  std::printf("%zu:recv:", this->time_);
  this->reciever_.Print();
  std::printf(" --> [%s]\n", static_cast<std::string>(
      packet_->get_destination_address()).c_str());
}

MoveEvent::MoveEvent(const Time time, Node &node, Position new_position) :
    Event(time), node_(node), new_position_(new_position) { }

void MoveEvent::Execute() {
  node_.get_connection().position = new_position_;
}

void MoveEvent::Print() {
  std::printf("%zu:move:", this->time_);
  this->node_.Print();
  std::printf(" --> (%s)\n", static_cast<std::string>(new_position_).c_str());
}

UpdateConnectionsEvent::UpdateConnectionsEvent(const Time time,
    std::vector<std::unique_ptr<Node>> &nodes) : Event(time), nodes_(nodes) { }

void UpdateConnectionsEvent::Execute() {
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    nodes_[i]->UpdateConnections(nodes_);
  }
}

void UpdateConnectionsEvent::Print() {
  std::printf("%zu:update_connection:", this->time_);
}

}  // namespace simulation