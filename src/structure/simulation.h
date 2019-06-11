//
// simulation.h
//

#ifndef SARP_STRUCTURE_SIMULATION_H_
#define SARP_STRUCTURE_SIMULATION_H_

#include <iostream>
#include <memory>
#include <queue>
#include <ctime>
#include <cstdint>

#include "event.h"
#include "network.h"
#include "node.h"
#include "../network_generator/event_generator.h"

namespace simulation {

class Event;
class Network;
class EventGenerator;
class Simulation;

using Time = size_t;

class SimulationParameters {
 friend Simulation;
 friend std::ostream &operator<<(std::ostream &os,
    const SimulationParameters &simulation_parameters);
 public:
  Time DeliveryDuration(const Node &from, const Node &to,
      const std::size_t packet_size) const;

  Time get_duration() const;
  uint32_t get_ttl_limit() const;

 private:
  Time duration_ = 0;
  uint32_t ttl_limit_ = 0;
};

class Statistics {
 friend std::ostream &operator<<(std::ostream &os, const Statistics stats);
 public:
  void RegisterDeliveredPacket();
  void RegisterUndeliveredPacket();
  void RegisterTimedOutPacket();
  void RegisterHop();
  void RegisterRoutingOverheadSend();
  void RegisterRoutingOverheadDelivered();
  void RegisterRoutingOverheadSize(const std::size_t routing_packet_size);
  void RegisterDetectedCycle();
  double DensityOfNodes() const;
  double MeanNodeConnectivity() const;

  size_t get_delivered_packets_count() const;
  size_t get_undelivered_packets_count() const;
  size_t get_routing_overhead_send() const;
  size_t get_routing_overhead_delivered() const;
  size_t get_routing_overhead_size() const;
  size_t get_cycles_detected_count() const;

  const Network *network_ = nullptr;
 private:
  std::size_t delivered_packets_ = 0;
  std::size_t undelivered_packets_ = 0;
  std::size_t hops_count_ = 0;
  std::size_t timed_out_packets_ = 0;
  std::size_t routing_overhead_send_packets_ = 0;
  std::size_t routing_overhead_delivered_packets_ = 0;
  std::size_t routing_overhead_size_ = 0;
  std::size_t cycles_detected_ = 0;
};

class Simulation {
 friend class Event;
 public:
  static Simulation& set_properties(std::unique_ptr<Network> network,
      Time duration, uint32_t ttl_limit);
  static Simulation& get_instance();

  void Run(std::vector<std::unique_ptr<EventGenerator>> &event_generators);
  void ScheduleEvent(std::unique_ptr<Event> event);

  Time get_current_time() const;
  Statistics& get_statistics();
  const SimulationParameters& get_simulation_parameters() const;

 private:
  class EventComparer {
   public:
    bool operator()(const std::unique_ptr<Event> &lhs,
        const std::unique_ptr<Event> &rhs);
  };

  Simulation() = default;

  static inline Simulation* instance_ = nullptr;
  std::unique_ptr<Network> network_ = nullptr;
  SimulationParameters simulation_parameters_{};
  Statistics statistics_;
  Time time_;
  std::priority_queue<std::unique_ptr<Event>,
      std::vector<std::unique_ptr<Event>>, EventComparer> schedule_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_H_
