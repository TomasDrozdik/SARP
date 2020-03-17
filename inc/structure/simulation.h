//
// simulation.h
//

#ifndef SARP_STRUCTURE_SIMULATION_H_
#define SARP_STRUCTURE_SIMULATION_H_

#include <cstdint>
#include <ctime>
#include <iostream>
#include <memory>
#include <queue>

#include "network_generator/event_generator.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"

namespace simulation {

class Event;
class Network;
class EventGenerator;

using Time = std::size_t;

class Simulation {
  friend class Event;

 public:
  // Meyers singleton.
  static Simulation &get_instance();

  void Run(std::unique_ptr<Network> network,
           std::vector<std::unique_ptr<EventGenerator>> &event_generators);

  void ScheduleEvent(std::unique_ptr<Event> event);

  Time get_current_time() const { return time_; }

  // TODO: may be can be removed
  const Network &get_network() const { return *network_; }

 private:
  class EventComparer {
   public:
    bool operator()(const std::unique_ptr<Event> &lhs,
                    const std::unique_ptr<Event> &rhs);
  };

  static inline Simulation *instance_ = nullptr;  // Meyers singleton.
  std::unique_ptr<Network> network_ = nullptr;
  Time time_;
  std::priority_queue<std::unique_ptr<Event>,
                      std::vector<std::unique_ptr<Event>>, EventComparer>
      schedule_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_H_
