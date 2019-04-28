//
// events.cc
//

#include "events.h"

#include <cstdio>

#include "protocol_packet.h"
#include "simulation.h"
#include "interface.h"

namespace simulation {

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

}  // namespace simulation