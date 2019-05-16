//
// events.h
//

#ifndef SARP_SIMULATION_STRUCTURE_TASKS_H_
#define SARP_SIMULATION_STRUCTURE_TASKS_H_

#include <memory>

#include "simulation.h"
#include "node.h"

namespace simulation {

class Event {
 friend class Simulation;  // To adjust time if is_absolute_time is set.
 public:
  Event(const Time time, bool is_absolute_time = false);
  virtual ~Event() = 0;

  virtual void Execute() = 0;
  virtual void Print() = 0;
  bool operator<(const Event &other) const;

  Time get_time() const;

 protected:
  // Time in microseconds form the beginning of the program
  Time time_;
  const bool is_absolute_time_;
};

class EventGenerator {
 public:
  virtual std::unique_ptr<Event> operator++() = 0;
};

class SendEvent final : public Event {
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

class RecvEvent final : public Event {
 public:
  RecvEvent(const Time time, Interface &reciever,
      std::unique_ptr<ProtocolPacket> packet);
  ~RecvEvent() override = default;

  void Execute() override;
  void Print() override;
private:
  Interface &reciever_;
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