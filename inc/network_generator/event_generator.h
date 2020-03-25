//
// event_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_

#include <memory>
#include <vector>

#include "network_generator/position_generator.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"
#include "structure/simulation_parameters.h"

namespace simulation {

class Event;

using Time = std::size_t;

class EventGenerator {
 public:
  virtual ~EventGenerator() = default;

  // RETURNS: pointer to new Event or nullptr if generation ends.
  virtual std::unique_ptr<Event> operator++() = 0;

 protected:
  EventGenerator(Time start, Time end);
  Time start_, end_;
};

class TrafficGenerator : public EventGenerator {
 public:
  TrafficGenerator(Time start, Time end,
                   const std::vector<std::unique_ptr<Node>> &nodes,
                   std::size_t count, bool reflexive_trafic = true);
  ~TrafficGenerator() override = default;

  // Create new send event form random time in time interval and between random
  // two nodes.
  std::unique_ptr<Event> operator++() override;

 private:
  const std::vector<std::unique_ptr<Node>> &nodes_;
  std::size_t count_;
  bool reflexive_trafic_;
  std::size_t counter_ = 0;
};

class MoveGenerator : public EventGenerator {
 public:
  MoveGenerator(Time start, Time end, Time step_period, const Network &network,
                std::unique_ptr<PositionGenerator> direction_generator,
                double min_speed, double max_speed, Time min_pause,
                Time max_pause);
  ~MoveGenerator() override = default;

  std::unique_ptr<Event> operator++() override;

 private:
  struct MobilityPlan {
    Position current;
    Position destination;
    double speed;
    Time pause;
    bool is_paused = false;
  };

  void CreatePlan(std::size_t idx);

  // Makes one step in MobilityPlan for node on index idx.
  //
  // RETURNS: true if plan succeeded and new plan should be Created,
  // false otherwise.
  bool MakeStepInPlan(std::size_t idx);

  Time step_period_ = 1000;
  const Network &network_;
  std::unique_ptr<PositionGenerator> direction_generator_;
  double min_speed_;
  double max_speed_;
  Time min_pause_;
  Time max_pause_;

  Time virtual_time_;
  std::vector<MobilityPlan> plans_;
  std::size_t i_ = 0;    // internal counter for iteration over all nodes
  bool reset_ = false;   // signals whether intenal counter reached nodes.size()
  bool change_ = false;  // signals whether intenal counter reached nodes.size()
};

class RoutingPeriodicUpdateGenerator : public EventGenerator {
 public:
  RoutingPeriodicUpdateGenerator(Time start, Time end, Time period,
                                 Network &nodes);
  ~RoutingPeriodicUpdateGenerator() override = default;

  // First has to return nodes_.size() Routing::Init() to renew active
  // connections at periodic time. Then returns nodes_.size() Routing::Update()
  // all at time periodic time + 1 since Schedule does mix events with equal
  // execution time which is something we want to prevent.
  std::unique_ptr<Event> operator++() override;

 private:
  Time period_;
  Network &network_;

  Time virtual_time_;
  std::size_t i_ = 0;  // internal counter for Routing::UpdateInterfaces()
  std::size_t j_ = 0;  // internal counter for Routing::Update()
  bool update_interfaces = true;
};

class CustomEventGenerator : public EventGenerator {
 public:
  CustomEventGenerator(std::vector<std::unique_ptr<Event>> events);
  ~CustomEventGenerator() override = default;

  std::unique_ptr<Event> operator++() override;

 private:
  std::vector<std::unique_ptr<Event>> events_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
