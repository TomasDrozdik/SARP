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

#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"
#include "network_generator/event_generator.h"

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
  SimulationParameters(Time duration, uint32_t ttl_limit,
      uint32_t connection_range) :
          duration_(duration),
          ttl_limit_(ttl_limit),
          connection_range_(connection_range) { }

  Time DeliveryDuration(const Node &from, const Node &to,
      const std::size_t packet_size) const;

  Time get_duration() const {
    return duration_;
  }

  uint32_t get_ttl_limit() const {
    return ttl_limit_;
  }

  uint32_t get_connection_range() const {
    return connection_range_;
  }

 private:
  // Default ctor for initialization of private field in Simulation class.
  // No reference of pointer is used due to singleton properties and to preserve
  // constness and prevent -Wuninitialized
  SimulationParameters() = default;

  Time duration_ = 0;
  uint32_t ttl_limit_ = 0;
  uint32_t connection_range_ = 0;
};

class Statistics {
 friend std::ostream &operator<<(std::ostream &os, const Statistics stats);
 public:
  double DensityOfNodes() const;
  double MeanNodeConnectivity() const;

  void RegisterDeliveredPacket() {
    ++delivered_packets_;
  }

  void RegisterUndeliveredPacket() {
    ++undelivered_packets_;
  }

  void RegisterHop() {
    ++hops_count_;
  }

  void RegisterRoutingOverheadSend() {
    ++routing_overhead_send_packets_;
  }

  void RegisterRoutingOverheadDelivered() {
    ++routing_overhead_delivered_packets_;
  }

  void RegisterRoutingOverheadSize(
      const std::size_t routing_packet_size) {
    routing_overhead_size_ += routing_packet_size;
  }

  void RegisterBrokenConnectionsSend() {
    ++broken_connection_sends_;
  }

  void RegisterDetectedCycle() {
    ++cycles_detected_;
  }

  void RegisterTTLExpire() {
    ++ttl_expired_;
  }

  void set_network(const Network *network) {
    this->network_ = network;
  }

  size_t get_delivered_packets_count() const {
    return delivered_packets_;
  }

  size_t get_undelivered_packets_count() const {
    return undelivered_packets_;
  }

  size_t get_routing_overhead_send() const {
    return routing_overhead_send_packets_;
  }

  size_t get_routing_overhead_delivered() const {
    return routing_overhead_delivered_packets_;
  }

  size_t get_routing_overhead_size() const {
    return routing_overhead_size_;
  }

  size_t get_cycles_detected_count() const {
    return cycles_detected_;
  }
 private:
  const Network *network_ = nullptr;
  std::size_t delivered_packets_ = 0;
  std::size_t undelivered_packets_ = 0;
  std::size_t hops_count_ = 0;
  std::size_t routing_overhead_send_packets_ = 0;
  std::size_t routing_overhead_delivered_packets_ = 0;
  std::size_t routing_overhead_size_ = 0;
  std::size_t broken_connection_sends_ = 0;
  std::size_t cycles_detected_ = 0;
  std::size_t ttl_expired_ = 0;
};

class Simulation {
 friend class Event;
 public:
  // Meyers singleton.
  static Simulation& set_properties(std::unique_ptr<Network> network,
      SimulationParameters parameters);
  static Simulation& get_instance();

  void Run(std::vector<std::unique_ptr<EventGenerator>> &event_generators);
  void ScheduleEvent(std::unique_ptr<Event> event);

  Time get_current_time() const {
    return time_;
  }

  Statistics &get_statistics() {
    return statistics_;
  }

  const SimulationParameters &get_simulation_parameters() const {
    return simulation_parameters_;
  }

  const Network &get_network() const {
    return *network_;
  }

 private:
  class EventComparer {
   public:
    bool operator()(const std::unique_ptr<Event> &lhs,
        const std::unique_ptr<Event> &rhs);
  };

  static inline Simulation* instance_ = nullptr;  // Meyers singleton.
  std::unique_ptr<Network> network_ = nullptr;

  // Default initilized by private ctor using friendness, to prevent
  // -Wuninitilized, preserve constness and avoid pointer type, copy is cheap.
  SimulationParameters simulation_parameters_;

  Statistics statistics_;
  Time time_;
  std::priority_queue<std::unique_ptr<Event>,
      std::vector<std::unique_ptr<Event>>, EventComparer> schedule_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_H_
