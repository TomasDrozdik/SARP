//
// event.h
//

#ifndef SARP_STRUCTURE_TASKS_H_
#define SARP_STRUCTURE_TASKS_H_

#include <iostream>
#include <memory>

#include "network_generator/position_generator.h"
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
class Parameters;
class Node;

class Event {
  friend class Simulation;  // To adjust time if is_absolute_time is set.
  friend std::ostream &operator<<(std::ostream os, const Event &event);

 public:
  virtual ~Event() = default;

  // Execute is called when simulation reaches event's time.
  virtual void Execute(Env &env) = 0;

  virtual std::ostream &Print(std::ostream &os) const = 0;

  bool operator<(const Event &other) const;

  Time get_time() const { return time_; }

  bool IsAblsoluteTime() { return time_type_ == TimeType::ABSOLUTE; }

  bool IsRelativeTime() { return time_type_ == TimeType::RELATIVE; }

 protected:
  Event(Time time, TimeType time_type);

  // Return priority of the event. This priority is used in operator< to order
  // events not only based on time but also when the time is equal use this
  // priority.
  // Default is 0 but since int is used both directions are possible.
  virtual int get_priority() const { return 0; };

  Time time_;
  const TimeType time_type_;
};

class SendEvent final : public Event {
 public:
  // Sends prepared packet from given sender node. Generally used for routing
  // updates.
  SendEvent(Time time, TimeType time_type, Node &sender,
            std::unique_ptr<Packet> packet);

  // Sends new packet from sender to destination with given size. Used for non
  // routing related packets.
  SendEvent(Time time, TimeType time_type, Node &sender, Node &destination,
            uint32_t size);

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
  RecvEvent(Time time, TimeType time_type, Node &sender, Node &reciever,
            std::unique_ptr<Packet> packet);

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority higher than Move so that already received packets are
  // processed first.
  int get_priority() const override { return 90; }

 private:
  Node &sender_;
  Node &reciever_;
  std::unique_ptr<Packet> packet_;
};

class RandomTrafficEvent final : public Event {
 public:
  RandomTrafficEvent(Time time, TimeType time_type, Network &network);

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 private:
  Network &network_;
};

class TrafficEvent final : public Event {
 public:
  TrafficEvent(Time time, TimeType time_type, Network &network, NodeID from,
               NodeID to);

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 private:
  Network &network_;
  NodeID from_;
  NodeID to_;
};

class MoveEvent final : public Event {
 public:
  MoveEvent(Time time, TimeType time_type, Network &network, Node &node,
            std::unique_ptr<PositionGenerator> directions);

  void Execute(Env &env) override;

  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make this priority higher than UpdateNeighbors since we want to know about
  // new neighbors.
  int get_priority() const override { return 80; }

 private:
  bool AssignNewPlan(const Parameters &parameters);

  Network &network_;
  Node &node_;
  std::unique_ptr<PositionGenerator> directions_;
};

class UpdateNeighborsEvent final : public Event {
 public:
  UpdateNeighborsEvent(Time time, TimeType time_type, Network &network);

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority higher so that RoutinUpdate, Send and Recv events have proper
  // neighbor information.
  int get_priority() const override { return 50; }

 private:
  Network &network_;
};

class UpdateRoutingEvent final : public Event {
 public:
  UpdateRoutingEvent(Time time, TimeType time_type, Routing &routing);

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Routing &routing_;
};

class RequestUpdateEvent final : public Event {
 public:
  RequestUpdateEvent(Time time, TimeType time_type, Node *node, Node *neighbor);

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Node *node_;
  Node *neighbor_;
};

class BootEvent final : public Event {
 public:
  BootEvent(Time time, TimeType time_type, Network &network,
            std::unique_ptr<Node> node,
            std::unique_ptr<PositionGenerator> directions);

  void Execute(Env &env);

  std::ostream &Print(std::ostream &os) const;

 protected:
  // Boot node has a top priority as an initilization event.
  int get_priority() const override { return 100; }

 private:
  Network &network_;
  std::unique_ptr<Node> node_;
  std::unique_ptr<PositionGenerator> directions_;
};

class ReaddressEvent final : public Event {
 public:
  ReaddressEvent(Time time, TimeType time_type, Network &network,
                 bool only_empty);

  void Execute(Env &env);

  std::ostream &Print(std::ostream &os) const;

 protected:
  int get_priority() const override { return 50; }

 private:
  Network &network_;
  const bool only_empty_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_TASKS_H_
