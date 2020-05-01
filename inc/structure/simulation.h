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

struct SimulationParameters final {
  friend class SimulationParametersBuilder;

  // Empty ctor usefull for Env setup.
  SimulationParameters() = default;

  SimulationParameters(
      RoutingType routing_type, uint32_t node_count, Time duration,
      uint32_t ttl_limit, uint32_t connection_range, Position position_min,
      Position position_max,
      std::unique_ptr<PositionGenerator> initial_positions, bool has_traffic,
      bool has_movement, bool has_periodic_routing_update, bool has_sarp,
      Time traffic_start, Time traffic_end, std::size_t traffic_event_count,
      Time move_start, Time move_end, Time move_step_period,
      double move_speed_min, double move_speed_max, Time move_pause_min,
      Time move_pause_max, Time neighbor_update_period,
      std::unique_ptr<PositionGenerator> move_directions,
      Time routing_update_period, Cost defualt_neighbor_cost,
      Cost defualt_reflexive_cost, double quantile_treshold);

  SimulationParameters(const SimulationParameters &other);

  SimulationParameters(SimulationParameters &&other);

  static Time DeliveryDuration(const Node &from, const Node &to,
                               const std::size_t packet_size);

  std::unique_ptr<PositionGenerator> get_initial_positions() const {
    return initial_positions_->Clone();
  }

  std::unique_ptr<PositionGenerator> get_move_directions() const {
    return move_directions_->Clone();
  }

  // General
  const RoutingType routing_type = RoutingType::SARP;
  const uint32_t node_count = 0;
  const Time duration = 0;
  const uint32_t ttl_limit = 0;
  const uint32_t connection_range = 0;
  const Position position_min = 0;
  const Position position_max = 0;

  const bool has_traffic = 0;
  const bool has_movement = 0;
  const bool has_periodic_routing_update = 0;
  const bool has_sarp = 0;

  // Traffic generation parameters.
  const Time traffic_start = 0;
  const Time traffic_end = 0;
  const std::size_t traffic_event_count = 0;

  // Movement simulation parameters.
  const Time move_start = 0;
  const Time move_end = 0;
  const Time move_step_period = 0;
  const double move_speed_min = 0;  // m/s
  const double move_speed_max = 0;  // m/s
  const Time move_pause_min = 0;
  const Time move_pause_max = 0;
  // Compute these positions from initial positions if not provided.
  const Time neighbor_update_period = 0;

  // Periodic routing update parameters.
  const Time routing_update_period = 0;

  // Sarp
  const Cost default_neighbor_cost = {.mean = 1, .sd = 0.1, .group_size = 1};
  const Cost default_reflexive_cost = {.mean = 0, .sd = 0.1, .group_size = 1};
  const double quantile_treshold = 0;

 private:
  // General
  const std::unique_ptr<PositionGenerator> initial_positions_ = nullptr;

  // Movement
  const std::unique_ptr<PositionGenerator> move_directions_ = nullptr;
};

std::ostream &operator<<(std::ostream &os, const SimulationParameters &sp);

class Simulation final {
  // Only env has access to private ctor to avoid pointer use.
  friend class Env;

 public:
  static std::pair<std::unique_ptr<Network>,
                   std::vector<std::unique_ptr<EventGenerator>>>
  CreateScenario(const SimulationParameters &sp);

  static void Run(Env &env, Network &network,
                  std::vector<std::unique_ptr<EventGenerator>> &events);

  void ScheduleEvent(std::unique_ptr<Event> event);

  Time get_current_time() const { return time_; }

 private:
  Simulation() = default;

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

  void Print(std::ostream &os, const Network &network);

  double DensityOfNodes(const Network &network);

  double MeanNodeConnectivity(const Network &network);

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
  Env(SimulationParameters sp) : parameters(sp) {}

  Simulation simulation;
  Statistics stats;
  const SimulationParameters parameters;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_H_
