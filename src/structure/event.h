//
// event.h
//

#ifndef SARP_STRUCTURE_TASKS_H_
#define SARP_STRUCTURE_TASKS_H_

#include <iostream>
#include <memory>

#include "simulation.h"
#include "node.h"

namespace simulation {

class Event {
 friend class Simulation;  // To adjust time if is_absolute_time is set.
 friend std::ostream &operator<<(std::ostream os, const Event &event);
 public:
  Event(const Time time, bool is_absolute_time);
  virtual ~Event() = 0;

  virtual void Execute() = 0;
  virtual std::ostream &Print(std::ostream &os) const = 0;
  bool operator<(const Event &other) const;

  Time get_time() const;

 protected:
  // Time in microseconds form the beginning of the program
  Time time_;
  const bool is_absolute_time_;
};

class SendEvent final : public Event {
 public:
  SendEvent(const Time time, bool is_absolute_time, Node &sender,
      std::unique_ptr<ProtocolPacket> packet);
  ~SendEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
private:
  Node &sender_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class RecvEvent final : public Event {
 public:
  RecvEvent(const Time time, bool is_absolute_time, Interface &reciever,
      std::unique_ptr<ProtocolPacket> packet);
  ~RecvEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
private:
  Interface &reciever_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class MoveEvent : public Event {
 public:
  MoveEvent(const Time time, bool is_absolute_time, Node &node,
      Position new_position);
  ~MoveEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
 private:
  Node &node_;
  Position new_position_;
};

class UpdateConnectionsEvent : public Event {
 public:
  UpdateConnectionsEvent(const Time time, bool is_absolute_time,
      Network &network);
  ~UpdateConnectionsEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
 private:
  Network &network_;
};

class InitRoutingEvent : public Event {
 public:
  InitRoutingEvent(const Time time, bool is_absolute_time, Routing &routing);
  ~InitRoutingEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
 private:
  Routing &routing_;
};

class UpdateRoutingEvent : public Event {
 public:
  UpdateRoutingEvent(const Time time, bool is_absolute_time, Routing &routing);
  ~UpdateRoutingEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
 private:
  Routing &routing_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_TASKS_H_