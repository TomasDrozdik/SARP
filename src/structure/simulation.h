//
// simulation.h
//

#ifndef SARP_SIMULATION_STRUCTURE_SIMULATION_H_
#define SARP_SIMULATION_STRUCTURE_SIMULATION_H_

#include <memory>
#include <queue>

#include <ctime>

#include "event.h"
#include "network.h"
#include "node.h"
#include "statistics.h"

namespace simulation {

class Event;
class Network;
class SimulationParameters;
class Statistics;

using Time = size_t;

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

  void ScheduleEvent(std::unique_ptr<Event> event);
  void Run(std::vector<std::unique_ptr<Event>> starting_events);

  Time get_current_time() const;
  Statistics& get_statistics();
  const SimulationParameters& get_simulation_parameters() const;

 private:
  class EventComparer {
   public:
    bool operator()(const std::unique_ptr<Event> &lhs,
        const std::unique_ptr<Event> &rhs);
  };

  Simulation(std::unique_ptr<Network> network, Time duration,
      uint32_t signal_speedMbs);
  ~Simulation() = default;

  static inline Simulation* instance_ = nullptr;
  std::unique_ptr<Network> network_ = nullptr;
  std::unique_ptr<SimulationParameters> simulation_parameters_;
  std::unique_ptr<Statistics> statistics_;
  Time time_;

  std::priority_queue<
      Event*, std::vector<std::unique_ptr<Event>>, EventComparer> schedule_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_SIMULATION_H_