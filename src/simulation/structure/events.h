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

class MoveEvent : public Event {
 public:
  MoveEvent(const Time time, Node &node, Position new_position);
  ~MoveEvent() override = default;

  void Execute() override;
  void Print() override;
 private:
  Node &node_;
  Position new_position_;
};

class UpdateConnectionsEvent : public Event {
 public:
  UpdateConnectionsEvent(const Time time,
      std::vector<std::unique_ptr<Node>> &nodes);
  ~UpdateConnectionsEvent() override = default;

  void Execute() override;
  void Print() override;
 private:
  std::vector<std::unique_ptr<Node>> &nodes_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_TASKS_H_