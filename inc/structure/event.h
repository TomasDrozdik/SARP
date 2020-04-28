//
// event.h
//

#ifndef SARP_STRUCTURE_TASKS_H_
#define SARP_STRUCTURE_TASKS_H_

#include <iostream>
#include <memory>

#include "structure/network.h"
#include "structure/node.h"
#include "structure/packet.h"
#include "structure/position.h"
#include "structure/routing.h"
#include "structure/simulation.h"

namespace simulation {

class Network;
class Routing;
class Packet;
struct Env;
class Node;

using Time = std::size_t;

enum class TimeType {
  ABSOLUTE,
  RELATIVE,
};

class Event {
  friend class Simulation;  // To adjust time if is_absolute_time is set.
  friend std::ostream &operator<<(std::ostream os, const Event &event);

 public:
  virtual ~Event() = 0;

  // Execute is called when simulation reaches event's time.
  virtual void Execute(Env &env) = 0;

  virtual std::ostream &Print(std::ostream &os) const = 0;

  bool operator<(const Event &other) const;

  Time get_time() const { return time_; }

  bool IsAblsoluteTime() { return time_type_ == TimeType::ABSOLUTE; }

  bool IsRelativeTime() { return time_type_ == TimeType::RELATIVE; }

 protected:
  Event(const Time time, TimeType time_type);

  // Return priority of the event. This priority is used in operator< to order
  // events not only based on time but also when the time is equal use this
  // priority.
  // Default is 0 but since int is used both directions are possible.
  virtual int get_priority() const { return 0; };

  Time time_;
  const TimeType time_type_;
};

class InitNetworkEvent final : public Event {
 public:
  InitNetworkEvent(const Time time, TimeType time_type, Network &network);

  ~InitNetworkEvent() override = default;

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority higher so that RoutinUpdate, Send and Recv events have proper
  // neighbor information.
  int get_priority() const override { return 100; }

 private:
  Network &network_;
};

class SendEvent final : public Event {
 public:
  // Sends prepared packet from given sender node. Generally used for routing
  // updates.
  SendEvent(const Time time, TimeType time_type, Node &sender,
            std::unique_ptr<Packet> packet);

  // Sends new packet from sender to destination with given size. Used for non
  // routing related packets.
  SendEvent(const Time time, TimeType time_type, Node &sender,
            Node &destination, uint32_t size);

  ~SendEvent() override = default;

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 private:
  Node &sender_;
  Node *destination_ = nullptr;
  uint32_t size_;
  std::unique_ptr<Packet> packet_ = nullptr;
};

class RecvEvent final : public Event {
 public:
  RecvEvent(const Time time, TimeType time_type, Node &sender, Node &reciever,
            std::unique_ptr<Packet> packet);
  ~RecvEvent() override = default;

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Node &sender_;
  Node &reciever_;
  std::unique_ptr<Packet> packet_;
};

class MoveEvent final : public Event {
 public:
  MoveEvent(const Time time, TimeType time_type, Node &node, Network &network,
            Position position);
  ~MoveEvent() override = default;

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority lower so that RecvEvent can be processed, which requires
  // recieving and sending node to be connected. Move can change this so put
  // it's priority lower.
  int get_priority() const override { return -10; }

 private:
  Node &node_;
  Network &network_;
  Position new_position_;
};

class UpdateNeighborsEvent final : public Event {
 public:
  UpdateNeighborsEvent(const Time time, TimeType time_type, Network &network);
  ~UpdateNeighborsEvent() override = default;

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority higher so that RoutinUpdate, Send and Recv events have proper
  // neighbor information.
  int get_priority() const override { return 10; }

 private:
  Network &network_;
};

class UpdateRoutingEvent final : public Event {
 public:
  UpdateRoutingEvent(const Time time, TimeType time_type, Routing &routing);
  ~UpdateRoutingEvent() override = default;

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Routing &routing_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_TASKS_H_
