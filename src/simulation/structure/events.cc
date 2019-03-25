//
// events.cc
//

#include "events.h"

#include "protocol_packet.h"
#include "interface.h"

namespace simulation {

SendEvent::SendEvent(const Time time, Node &sender,
    std::unique_ptr<ProtocolPacket> &&packet) : Event(time), sender_(sender),
    packet_(std::move(packet)) { }

void SendEvent::execute() {
  sender_.Send(std::move(packet_));
}

RecvEvent::RecvEvent(const Time time, Node &reciever,
    std::unique_ptr<ProtocolPacket> &&packet) :
        Event(time), reciever_(reciever), packet_(std::move(packet)) { }

void RecvEvent::execute() {
  reciever_.Recv(std::move(packet_));
}

ProcessEvent::ProcessEvent(const Time time, Node &node) : Event(time),
    node_(node) { }

void ProcessEvent::execute() {
  node_.Process();
}

}  // namespace simulation