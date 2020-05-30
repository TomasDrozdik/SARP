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
class BootEvent;
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

  // Create new send event form random time in time interval and between random
  // two nodes.
  std::unique_ptr<Event> Next() override;

 private:
  range<Time> time_;
  Network &network_;
  std::size_t count_;
};

class NeighborUpdateGenerator final : public EventGenerator {
 public:
  NeighborUpdateGenerator(range<Time> time, Time period, Network &nodes);

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

  static std::unique_ptr<BootEvent> CreateBootEvent(Time time,
    TimeType time_type, Network &network, RoutingType routing,
    Position position, Address address, std::unique_ptr<PositionGenerator> directions);

 private:
  Network &network_;
  std::size_t count_;
  RoutingType routing_;
  std::unique_ptr<TimeGenerator> time_generator_;
  std::unique_ptr<PositionGenerator> pos_generator_;
  std::unique_ptr<AddressGenerator> address_generator_;
};

class ReaddressEventGenerator final : public EventGenerator {
 public:
  ReaddressEventGenerator(range<Time> time, Time period, Network &nodes);

  std::unique_ptr<Event> Next() override;

 private:
  const range<Time> time_;
  const Time period_;  // With period_ set to 0 no events are created.
  Time virtual_time_;
  Network &network_;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
