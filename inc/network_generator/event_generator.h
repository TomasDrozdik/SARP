//
// event_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_

#include <memory>
#include <vector>

#include "network_generator/position_generator.h"
#include "network_generator/time_generator.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"
#include "structure/position.h"
#include "structure/simulation.h"

namespace simulation {

class Event;
class Network;

using Time = std::size_t;

class EventGenerator {
 public:
  virtual ~EventGenerator() = default;

  // RETURNS: pointer to new Event or nullptr if generation ends.
  virtual std::unique_ptr<Event> Next() = 0;
};

class TrafficGenerator final : public EventGenerator {
 public:
  TrafficGenerator(range<Time> time, Network &network, std::size_t count);

  ~TrafficGenerator() override = default;

  // Create new send event form random time in time interval and between random
  // two nodes.
  std::unique_ptr<Event> Next() override;

 private:
  range<Time> time_;
  Network &network_;
  std::size_t count_;
};

class MoveGenerator final : public EventGenerator {
 public:
  MoveGenerator(range<Time> time, Time step_period, Network &network,
                std::unique_ptr<PositionGenerator> direction_generator,
                range<double> speed, range<Time> pause);

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

  range<Time> time_;
  Time step_period_ = 1000;
  Network &network_;
  std::unique_ptr<PositionGenerator> direction_generator_;
  range<double> speed_;
  range<Time> pause_;

  Time virtual_time_;
  std::vector<MobilityPlan> plans_;
  std::size_t i_ = 0;  // Internal counter for iteration over all nodes.
};

class NeighborUpdateGenerator final : public EventGenerator {
 public:
  // No start is specified since InitNetworkEvent does initial neighbor update
  // at time 0 so we start at time period.
  NeighborUpdateGenerator(range<Time> time, Time period, Network &nodes);
  ~NeighborUpdateGenerator() override = default;

  std::unique_ptr<Event> Next() override;

 private:
  const range<Time> time_;
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

class NodeGenerator final : public EventGenerator {
 public:
  NodeGenerator(Network &network, std::size_t count, RoutingType routing,
                std::unique_ptr<TimeGenerator> time_generator,
                std::unique_ptr<PositionGenerator> pos_generator,
                std::unique_ptr<AddressGenerator> address_generator);

  std::unique_ptr<Event> Next();

 private:
  Network &network_;
  std::size_t count_;
  RoutingType routing_;
  std::unique_ptr<TimeGenerator> time_generator_;
  std::unique_ptr<PositionGenerator> pos_generator_;
  std::unique_ptr<AddressGenerator> address_generator_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
