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
  SendEvent(const Time time, const Node &sender,
      std::unique_ptr<ProtocolPacket> packet);
  ~SendEvent() override = default;

  void Execute() override;
  void Print() override;
private:
  const Node &sender_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class RecvEvent : public Event {
 public:
  RecvEvent(const Time time, const Node &reciever,
      std::unique_ptr<ProtocolPacket> packet);
  ~RecvEvent() override = default;

  void Execute() override;
  void Print() override;
private:
  const Node &reciever_;
  std::unique_ptr<ProtocolPacket> packet_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_TASKS_H_