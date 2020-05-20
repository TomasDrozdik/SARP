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

#include "network_generator/address_generator.h"
#include "network_generator/event_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/cost.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/node.h"

namespace simulation {

class Event;
class Network;
class EventGenerator;
struct Env;

using Time = std::size_t;

enum class RoutingType {
  STATIC,
  DISTANCE_VECTOR,
  SARP,
};

static constexpr int W = 10;

std::ostream &operator<<(std::ostream &os, const RoutingType &r);

template <typename T>
using range = std::pair<T, T>;

template <typename T>
std::ostream &operator<<(std::ostream &os, const range<T> &r) {
  return os << r.first << ',' << r.second;
}

struct Parameters final {
  friend std::ostream &operator<<(std::ostream &os, const Parameters &p);

  struct General {
    General() = default;
    General(const General &other);
    General(General &&other) = default;
    General &operator=(const General &other);
    General &operator=(General &&other) = default;
    ~General() = default;

    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    RoutingType routing_type = RoutingType::SARP;
    uint32_t node_count = 0;
    Time duration = 0;
    uint32_t ttl_limit = 0;
    uint32_t connection_range = 0;
    range<Position> position_boundaries = {Position(0, 0, 0),
                                           Position(0, 0, 0)};
    std::unique_ptr<AddressGenerator> initial_addresses = nullptr;
    std::unique_ptr<PositionGenerator> initial_positions = nullptr;
  };

  struct Traffic {
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    range<Time> time_range = {0, 0};
    std::size_t event_count = 0;
  };

  struct Movement {
    Movement() = default;
    Movement(const Movement &other);
    Movement(Movement &&other) = default;
    Movement &operator=(const Movement &other);
    Movement &operator=(Movement &&other) = default;
    ~Movement() = default;

    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    range<Time> time_range = {0, 0};
    Time step_period = 0;
    range<double> speed_range = {0, 0};  // m/s
    range<Time> pause_range = {0, 0};
    Time neighbor_update_period = 0;
    std::unique_ptr<PositionGenerator> directions = nullptr;
  };

  struct PeriodicRouting {
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    Time update_period = 0;
  };

  struct Sarp {
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    Cost neighbor_cost{1, 0.1};
    Cost reflexive_cost{0, 0.1};
    Cost max_cost{std::numeric_limits<double>::max(), 0};
    double compact_treshold = 2;
    double update_treshold = 0.9;
    double min_standard_deviation = 0.1;
  };


  static void PrintCsvHeader(std::ostream &os);
  
  void PrintCsv(std::ostream &os) const;


  Parameters &AddGeneral(General parameters) {
    general_ = {true, parameters};
    return *this;
  }

  Parameters &AddTraffic(Traffic parameters) {
    traffic_ = {true, parameters};
    return *this;
  }

  Parameters &AddMovement(Movement parameters) {
    movement_ = {true, parameters};
    return *this;
  }

  Parameters &AddPeriodicRouting(PeriodicRouting parameters) {
    periodic_routing_ = {true, parameters};
    return *this;
  }

  Parameters &AddSarp(Sarp parameters) {
    sarp_parameters_ = {true, parameters};
    return *this;
  }

  RoutingType get_routing_type() const {
    assert(general_.first);
    return general_.second.routing_type;
  }

  bool has_traffic() const { return traffic_.first; }
  bool has_movement() const { return movement_.first; }
  bool has_periodic_routing() const { return periodic_routing_.first; }
  bool has_sarp() const { return sarp_parameters_.first; }

  uint32_t get_node_count() const {
    assert(general_.first);
    return general_.second.node_count;
  }

  Time get_duration() const {
    assert(general_.first);
    return general_.second.duration;
  }

  uint32_t get_ttl_limit() const {
    assert(general_.first);
    return general_.second.ttl_limit;
  }

  uint32_t get_connection_range() const {
    assert(general_.first);
    return general_.second.connection_range;
  }

  range<Position> get_position_boundaries() const {
    assert(general_.first);
    return general_.second.position_boundaries;
  }

  std::unique_ptr<AddressGenerator> get_initial_addresses() const {
    assert(general_.first);
    return (general_.second.initial_addresses)
               ? general_.second.initial_addresses->Clone()
               : nullptr;
  }

  std::unique_ptr<PositionGenerator> get_initial_positions() const {
    assert(general_.first);
    return general_.second.initial_positions->Clone();
  }

  range<Time> get_traffic_time_range() const {
    assert(traffic_.first);
    return traffic_.second.time_range;
  }

  std::size_t get_traffic_event_count() const {
    assert(traffic_.first);
    return traffic_.second.event_count;
  }

  range<Time> get_move_time_range() const {
    assert(movement_.first);
    return movement_.second.time_range;
  }

  Time get_move_step_period() const {
    assert(movement_.first);
    return movement_.second.step_period;
  }

  range<double> get_move_speed_range() const {
    assert(movement_.first);
    return movement_.second.speed_range;
  }

  range<Time> get_move_pause_range() const {
    assert(movement_.first);
    return movement_.second.pause_range;
  }

  Time get_neighbor_update_period() const {
    assert(movement_.first);
    return movement_.second.neighbor_update_period;
  }

  std::unique_ptr<PositionGenerator> get_move_directions() const {
    assert(movement_.first);
    if (movement_.second.directions) {
      return movement_.second.directions->Clone();
    }
    return std::make_unique<RandomPositionGenerator>(
        get_position_boundaries().first, get_position_boundaries().second);
  }

  Time get_routing_update_period() const {
    assert(periodic_routing_.first);
    return periodic_routing_.second.update_period;
  }

  const Parameters::Sarp &get_sarp_parameters() const {
    assert(sarp_parameters_.first);
    return sarp_parameters_.second;
  }

 private:
  std::pair<bool, General> general_ = {false, General()};
  std::pair<bool, Traffic> traffic_ = {false, Traffic()};
  std::pair<bool, Movement> movement_ = {false, Movement()};
  std::pair<bool, PeriodicRouting> periodic_routing_ = {false,
                                                        PeriodicRouting()};
  std::pair<bool, Sarp> sarp_parameters_ = {false, Sarp()};
};

std::ostream &operator<<(std::ostream &os, const Cost &cost);
std::ostream &operator<<(std::ostream &os, const Parameters::General &p);
std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p);
std::ostream &operator<<(std::ostream &os,
                         const Parameters::PeriodicRouting &p);
std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p);
std::ostream &operator<<(std::ostream &os, const Parameters &p);

class Simulation final {
 public:
  static std::pair<std::unique_ptr<Network>,
                   std::vector<std::unique_ptr<EventGenerator>>>
  CreateScenario(const Parameters &sp);

  static void Run(Env &env, Network &network,
                  std::vector<std::unique_ptr<EventGenerator>> &events);

  void ScheduleEvent(std::unique_ptr<Event> event);

  Time get_current_time() const { return time_; }

 private:
  class EventComparer {
   public:
    bool operator()(const std::unique_ptr<Event> &lhs,
                    const std::unique_ptr<Event> &rhs);
  };

  void InitSchedule(Network &network,
                    std::vector<std::unique_ptr<EventGenerator>> &events);

  void Start(Env &env, Network &network);

  Time time_;
  std::priority_queue<std::unique_ptr<Event>,
                      std::vector<std::unique_ptr<Event>>, EventComparer>
      schedule_;
};

class Statistics final {
 public:
  void Reset();

  static void PrintCsvHeader(std::ostream &os);

  static std::size_t CountRoutingRecords(const Network &network);

  void PrintCsv(std::ostream &os, const Network &network) const;

  void Print(std::ostream &os, const Network &network) const;

  double DensityOfNodes(const Network &network) const;

  double MeanNodeConnectivity(const Network &network) const;

  void RegisterDeliveredPacket() { ++delivered_packets_; }

  void RegisterDataPacketLoss() { ++data_packets_lost_; }

  void RegisterHop() { ++hops_count_; }

  void RegisterRoutingOverheadSend() { ++routing_overhead_send_packets_; }

  void RegisterRoutingOverheadLoss() { ++routing_overhead_lost_packets_; }

  void RegisterRoutingOverheadDelivered() {
    ++routing_overhead_delivered_packets_;
  }

  void RegisterRoutingOverheadSize(const std::size_t routing_packet_size) {
    routing_overhead_size_ += routing_packet_size;
  }

  void RegisterBrokenConnectionSend() { ++broken_connection_sends_; }

  void RegisterDetectedCycle() { ++cycles_detected_; }

  void RegisterTTLExpire() { ++ttl_expired_; }

  void RegisterRoutingUpdateFromNonNeighbor() {
    ++routing_update_from_non_neighbor;
  }

  void RegisterRoutingResultNotNeighbor() { ++routing_result_not_neighbor_; }

  void RegisterInvalidRoutingMirror() { ++routing_mirror_not_valid_; }

  void RegisterSendEvent() { ++send_event_; }

  void RegisterRecvEvent() { ++recv_event_; }

  void RegisterMoveEvent() { ++move_event_; }

  void RegisterUpdateNeighborsEvent() { ++update_neighbors_event_; }

  void RegisterUpdateRoutingEvent() { ++update_routing_event_; }

  void RegisterUpdateRoutingCall() { ++update_routing_calls_; }

  void RegisterCheckUpdateRoutingCall() { ++check_update_routing_calls_; }

  void RegisterRoutingRecordDeletion() { ++routing_record_deletion_; }

  void RegisterReflexiveRoutingResult() { ++reflexive_routing_result_; }

 private:
  std::size_t delivered_packets_ = 0;
  std::size_t data_packets_lost_ = 0;
  std::size_t hops_count_ = 0;

  std::size_t routing_overhead_send_packets_ = 0;
  std::size_t routing_overhead_lost_packets_ = 0;
  std::size_t routing_overhead_delivered_packets_ = 0;
  std::size_t routing_overhead_size_ = 0;

  std::size_t broken_connection_sends_ = 0;
  std::size_t cycles_detected_ = 0;
  std::size_t ttl_expired_ = 0;
  std::size_t routing_update_from_non_neighbor = 0;
  std::size_t routing_result_not_neighbor_ = 0;
  std::size_t routing_mirror_not_valid_ = 0;

  std::size_t send_event_ = 0;
  std::size_t recv_event_ = 0;
  std::size_t move_event_ = 0;
  std::size_t update_neighbors_event_ = 0;
  std::size_t update_routing_event_ = 0;

  std::size_t update_routing_calls_ = 0;
  std::size_t check_update_routing_calls_ = 0;

  std::size_t routing_record_deletion_ = 0;
  std::size_t reflexive_routing_result_ = 0;
};

struct Env {
  Simulation simulation;
  Statistics stats;
  Parameters parameters;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_H_
