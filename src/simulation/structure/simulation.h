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
class SimulationParameters;
class Statistics;

using Time = size_t;

class Event {
 friend class Simulation;  // To adjust time if is_absolute_time is set.
 public:
  class EventComparer {
   public:
    bool operator()(const Event *lhs, const Event *rhs);
  };

  Event(const Time time, bool is_absolute_time = false);
  virtual ~Event() = 0;

  virtual void Execute() = 0;
  virtual void Print() = 0;
  bool operator<(const Event &other) const;

  Time get_time() const;

 protected:
  // Time in microseconds form the beginning of the program
  Time time_;
  const bool is_absolute_time_;
};

class SimulationParameters {
 public:
  SimulationParameters(Time simulation_duration, int signal_speed_Mbps);
  ~SimulationParameters() = default;

  void Print() const;
  Time DeliveryDuration(const Node &from, const Node &to,
      const std::size_t packet_size) const;

  const Time simulation_duration;
  const int signal_speed_Mbps;
};

class Simulation {
 friend class Event;
 public:
  static Simulation& set_instance(std::unique_ptr<Network> network, Time
      duration, uint32_t signal_speedMbs);
  // Throws: iff Simulation is not initialized with factory throws
  //         std::logic_error.
  static Simulation& get_instance();

  void ScheduleEvent(Event *event);
  void Run();

  Time get_current_time() const;
  Statistics& get_statistics();
  const SimulationParameters& get_simulation_parameters() const;

 private:
  Simulation(std::unique_ptr<Network> network, Time duration,
      uint32_t signal_speedMbs);
  ~Simulation() = default;

  static inline Simulation* instance = nullptr;
  std::unique_ptr<Network> network_ = nullptr;
  std::unique_ptr<SimulationParameters> simulation_parameters_;
  std::unique_ptr<Statistics> statistics_;
  Time time_;

  // TODO: not all should be deleted
  // This is a priority queue of raw pointers their
  // deletion. That is due to inability to make priority_queue consisting of
  // unique_ptr<Event>.
  std::priority_queue<
      Event*, std::vector<Event*>, Event::EventComparer> schedule_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_SIMULATION_H_
