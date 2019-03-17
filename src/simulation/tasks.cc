//
// tasks.cc
//

#include "tasks.h"

#include "protocol_packet.h"
#include "interface.h"

namespace simulation {

SendTask::SendTask(const Time time, Node &sender,
    std::unique_ptr<ProtocolPacket> &&packet) : Task(time), sender_(sender),
    packet_(std::move(packet)) { }

void SendTask::execute() {
  sender_.Send(std::move(packet_));
}

RecvTask::RecvTask(const Time time, Node &reciever,
    std::unique_ptr<ProtocolPacket> &&packet) : Task(time), reciever_(reciever),
    packet_(std::move(packet)) { }

void RecvTask::execute() {
  reciever_.Recv(std::move(packet_));
}

ProcessTask::ProcessTask(const Time time, Node &node) : Task(time),
    node_(node) { }

void ProcessTask::execute() {
  node_.Process();
}

}  // namespace simulation