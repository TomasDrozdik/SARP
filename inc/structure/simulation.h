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
#include "structure/types.h"

namespace simulation {

struct Env;
class Network;
class Event;
class EventGenerator;

std::ostream &operator<<(std::ostream &os, const RoutingType &r);

struct Parameters final {
  friend std::ostream &operator<<(std::ostream &os, const Parameters &p);

  struct General {
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    Time duration = 0;
    uint32_t ttl_limit = 0;
    uint32_t connection_range = 0;
    Time neighbor_update_period = 0;
    Time routing_update_period = 0;
    range<Position> boundaries = {Position(0, 0, 0), Position(0, 0, 0)};
  };

  struct NodeGeneration {
    NodeGeneration() = default;
    NodeGeneration(NodeGeneration &&other) = default;
    NodeGeneration &operator=(NodeGeneration &&other) = default;
    ~NodeGeneration() = default;

    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    uint32_t node_count = 0;
    RoutingType routing_type = RoutingType::SARP;
    std::unique_ptr<TimeGenerator> boot_time = nullptr;
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
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    Time end = 0;
    Time step_period = 0;
    range<double> speed_range = {0, 0};  // m/s
    range<Time> pause_range = {0, 0};
    std::unique_ptr<PositionGenerator> directions = nullptr;
  };

  struct Sarp {
    static void PrintCsvHeader(std::ostream &os);
    void PrintCsv(std::ostream &os) const;

    Cost neighbor_cost{1, 0.1};
    double compact_treshold = 3;
    double update_treshold = 0.9;
    double min_standard_deviation = 0.1;
  };


  static void PrintCsvHeader(std::ostream &os);
  void PrintCsv(std::ostream &os) const;

  void AddGeneral(General parameters) { general_ = {true, parameters}; }
  bool has_general() const { return general_.first; }
  const General &get_general() const {
    assert(has_general());
    return general_.second;
  }

  void AddNodeGeneration(NodeGeneration parameters) {
    node_generation_ = {true, std::move(parameters)};
  } 
  bool has_node_generation() const { return node_generation_.first; }
  const NodeGeneration &get_node_generation() const {
    assert(has_node_generation());
    return node_generation_.second;
  }

  void AddTraffic(Traffic parameters) { traffic_ = {true, parameters}; }
  bool has_traffic() const { return traffic_.first; }
  const Traffic &get_traffic() const {
    assert(has_traffic());
    return traffic_.second;
  }

  void AddMovement(Movement parameters) {
    movement_ = {true, std::move(parameters)};
  }
  bool has_movement() const { return movement_.first; }
  const Movement &get_movement() const {
    assert(has_movement());
    return movement_.second;
  }

  void AddSarp(Sarp parameters) { sarp_parameters_ = {true, parameters}; }
  bool has_sarp() const { return sarp_parameters_.first; }
  const Sarp &get_sarp_parameters() const {
    assert(has_sarp());
    return sarp_parameters_.second;
  }

 private:
  std::pair<bool, General> general_ = {false, General()};
  std::pair<bool, Traffic> traffic_ = {false, Traffic()};
  std::pair<bool, Movement> movement_ = {false, Movement()};
  std::pair<bool, NodeGeneration> node_generation_ = {false, NodeGeneration()};
  std::pair<bool, Sarp> sarp_parameters_ = {false, Sarp()};
};

std::ostream &operator<<(std::ostream &os, const Cost &cost);
std::ostream &operator<<(std::ostream &os, const Parameters::General &p);
std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p);
std::ostream &operator<<(std::ostream &os, const Parameters::NodeGeneration &p);
std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p);
std::ostream &operator<<(std::ostream &os, const Parameters &p);

class Simulation final {
 public:
  static std::pair<std::unique_ptr<Network>,
                   std::vector<std::unique_ptr<EventGenerator>>>
  CreateScenario(const Parameters &sp);

  static void Run(Env &env, Network &network, unsigned seed,
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
