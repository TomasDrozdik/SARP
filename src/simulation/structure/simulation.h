//
// simulation.h
//

#ifndef SARP_SIMULATION_STRUCTURE_SIMULATION_H_
#define SARP_SIMULATION_STRUCTURE_SIMULATION_H_

#include <memory>
#include <queue>

#include <ctime>

#include "network.h"
#include "node.h"
#include "statistics.h"

namespace simulation {

class Network;
class AbstractNetworkFactory;
class SimulationParameters;
class Statistics;

using Time = std::size_t;

class Event {
 friend class Simulation;  // To adjust time if is_absolute_time is set.
 public:
  class EventComparer {
   public:
    bool operator()(const Event *lhs, const Event *rhs);
  };

  Event(const Time time, bool is_absolute_time = false);
  virtual ~Event() = 0;
  virtual void execute() = 0;
  bool operator<(const Event &other) const;

  Time get_time() const;

 protected:
  // Time in microseconds form the beginning of the program
  Time time_;
  const bool is_absolute_time_;
};

class Simulation {
 friend class Event;
 public:
  static Simulation& get_instance(
      const AbstractNetworkFactory &network_factory);
  // Throws: iff Simulation is no iniitalized with factory throws
  //         std::logic_error.
  static Simulation& get_instance();

  void ScheduleEvent(Event *event);
  void Run();

  Time get_current_time() const;
  Statistics& get_statistics();
  const SimulationParameters& get_simulation_parameters() const;

 private:
  Simulation(const AbstractNetworkFactory &network_factory);
  ~Simulation() = default;

  static inline Simulation* instance = nullptr;
  std::unique_ptr<Network> network_ = nullptr;
  Time time_;

  // This is a priority queue of raw pointers responsibe also for their
  // deletion. That is due to inability to make priority_queue consisting of
  // unique_ptr<Event>.
  std::priority_queue<
      Event*, std::vector<Event*>, Event::EventComparer> schedule_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_SIMULATION_H_
