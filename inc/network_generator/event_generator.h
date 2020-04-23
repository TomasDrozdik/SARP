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
#include "structure/position.h"
#include "structure/simulation_parameters.h"

namespace simulation {

class Event;
class Network;

using Time = std::size_t;

class EventGenerator {
 public:
  virtual ~EventGenerator() = default;

  // RETURNS: pointer to new Event or nullptr if generation ends.
  virtual std::unique_ptr<Event> Next() = 0;

 protected:
  EventGenerator(Time start, Time end);
  Time start_, end_;
};

class TrafficGenerator final : public EventGenerator {
 public:
  TrafficGenerator(Time start, Time end, std::vector<Node> &nodes,
                   std::size_t count);
  ~TrafficGenerator() override = default;

  // Create new send event form random time in time interval and between random
  // two nodes.
  std::unique_ptr<Event> Next() override;

 private:
  std::vector<Node> &nodes_;
  std::size_t count_;
  std::size_t counter_ = 0;
};

class MoveGenerator final : public EventGenerator {
 public:
  MoveGenerator(Time start, Time end, Time step_period, Network &network,
                std::unique_ptr<PositionGenerator> direction_generator,
                double min_speed, double max_speed, Time min_pause,
                Time max_pause);
  ~MoveGenerator() override = default;

  std::unique_ptr<Event> Next() override;

 private:
  struct MobilityPlan {
    Position current_position;
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
  Network &network_;
  std::unique_ptr<PositionGenerator> direction_generator_;
  double min_speed_;
  double max_speed_;
  Time min_pause_;
  Time max_pause_;

  Time virtual_time_;
  std::vector<MobilityPlan> plans_;
  std::size_t i_ = 0;  // Internal counter for iteration over all nodes.
};

class NeighborUpdateGenerator final : public EventGenerator {
 public:
  // No start is specified since InitNetworkEvent does initial neighbor update
  // at time 0 so we start at time period.
  NeighborUpdateGenerator(Time period, Time end, Network &nodes);
  ~NeighborUpdateGenerator() override = default;

  std::unique_ptr<Event> Next() override;

 private:
  const Time period_;  // With period_ set to 0 no events are created.
  Time virtual_time_;
  Network &network_;
};

class CustomEventGenerator final : public EventGenerator {
 public:
  CustomEventGenerator(std::vector<std::unique_ptr<Event>> events);
  ~CustomEventGenerator() override = default;

  std::unique_ptr<Event> Next() override;

 private:
  std::vector<std::unique_ptr<Event>> events_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
