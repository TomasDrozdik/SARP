//
// event.h
//

#ifndef SARP_STRUCTURE_TASKS_H_
#define SARP_STRUCTURE_TASKS_H_

#include <iostream>
#include <memory>

#include "structure/interface.h"
#include "structure/network.h"
#include "structure/node.h"
#include "structure/protocol_packet.h"
#include "structure/routing.h"
#include "structure/simulation.h"
#include "structure/simulation_parameters.h"

namespace simulation {

class Network;
class Interface;
class Routing;
class ProtocolPacket;

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
  virtual void Execute() = 0;

  // TODO See if it is necessary so far not used.
  // virtual void PostProcess();

  virtual std::ostream &Print(std::ostream &os) const = 0;
  bool operator<(const Event &other) const;

  Time get_time() const { return time_; }

  bool IsAblsoluteTime() { return time_type_ == TimeType::ABSOLUTE; }

  bool IsRelativeTime() { return time_type_ == TimeType::RELATIVE; }

 protected:
  Event(const Time time, TimeType time_type);
  Time time_;
  const TimeType time_type_;
};

class SendEvent final : public Event {
 public:
  // Sends prepared packet from given sender node. Generally used for routing
  // updates.
  SendEvent(const Time time, TimeType time_type, Node &sender,
            std::unique_ptr<ProtocolPacket> packet);

  // Sends new packet from sender to destination with given size. Used for non
  // routing related packets.
  SendEvent(const Time time, TimeType time_type, Node &sender,
            Node &destination, uint32_t size);

  ~SendEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Node &sender_;
  Node *destination_ = nullptr;
  uint32_t size_;
  std::unique_ptr<ProtocolPacket> packet_ = nullptr;
};

class RecvEvent final : public Event {
 public:
  RecvEvent(const Time time, TimeType time_type, Interface &reciever,
            std::unique_ptr<ProtocolPacket> packet);
  ~RecvEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Interface &reciever_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class MoveEvent final : public Event {
 public:
  MoveEvent(const Time time, TimeType time_type, Node &node,
            Position new_position);
  ~MoveEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Node &node_;
  Position new_position_;
};

class UpdateInterfacesEvent final : public Event {
 public:
  UpdateInterfacesEvent(const Time time, TimeType time_type, Network &network);
  ~UpdateInterfacesEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Network &network_;
};

class UpdateRoutingInterfacesEvent final : public Event {
 public:
  UpdateRoutingInterfacesEvent(const Time time, TimeType time_type,
                               Routing &routing);
  ~UpdateRoutingInterfacesEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Routing &routing_;
};

class UpdateRoutingEvent final : public Event {
 public:
  UpdateRoutingEvent(const Time time, TimeType time_type, Routing &routing);
  ~UpdateRoutingEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;

 private:
  Routing &routing_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_TASKS_H_
