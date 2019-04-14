//
// events.h
//

#ifndef SARP_SIMULATION_STRUCTURE_TASKS_H_
#define SARP_SIMULATION_STRUCTURE_TASKS_H_

#include <memory>

#include "simulation.h"
#include "node.h"

namespace simulation {

class SendEvent : public Event {
 public:
  SendEvent(const Time time, Node &sender,
      std::unique_ptr<ProtocolPacket> packet);
  ~SendEvent() = default;

  void execute() override;
private:
  Node &sender_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class RecvEvent : public Event {
 public:
  RecvEvent(const Time time, Node &reciever,
      std::unique_ptr<ProtocolPacket> packet);
  ~RecvEvent() = default;

  void execute() override;
private:
  Node &reciever_;
  std::unique_ptr<ProtocolPacket> packet_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_TASKS_H_