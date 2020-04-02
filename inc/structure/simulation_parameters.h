//
// simulation_parameters.h
//

#ifndef SARP_STRUCTURE_SIMULATION_PARAMETERS_H_
#define SARP_STRUCTURE_SIMULATION_PARAMETERS_H_

#include <cstddef>
#include <ostream>

#include "structure/node.h"
#include "structure/position.h"

namespace simulation {

using Time = std::size_t;

class Node;

class SimulationParameters {
  // TODO: create method to parse these parameters form outside file
  // TODO: create method to create event_generators from these parameters
  // TODO: figure out a way for these to be const i.e. non static builder
  // pattern or something like that
 public:
  // Static class.
  SimulationParameters() = delete;

  static bool IsMandatoryInitialized() {
    return duration_ && ttl_limit_ && connection_range_;
  }

  static std::ostream &Print(std::ostream &os);

  static Time DeliveryDuration(const Node &from, const Node &to,
                               const std::size_t packet_size);

  static void set_node_count(uint32_t count) { node_count_ = count; }
  static uint32_t get_node_count() { return node_count_; }

  static void set_duration(Time time) { duration_ = time; }
  static Time get_duration() { return duration_; }

  static void set_ttl_limit(uint32_t limit) { ttl_limit_ = limit; }
  static uint32_t get_ttl_limit() { return ttl_limit_; }

  static void set_connection_range(uint32_t range) {
    connection_range_ = range;
  }
  static uint32_t get_connection_range() { return connection_range_; }

  static void set_traffic_start(Time time) { traffic_start_ = time; }
  static Time get_traffic_start() { return traffic_start_; }

  static void set_traffic_end(Time time) { traffic_end_ = time; }
  static Time get_traffic_end() { return traffic_end_; }

  static void set_traffic_event_count(std::size_t count) {
    traffic_event_count_ = count;
  }
  static Time get_traffic_event_count() { return traffic_event_count_; }

  static void set_reflexive_traffic(bool is_reflexive) {
    reflexive_traffic_ = is_reflexive;
  }
  static bool get_reflexive_traffic() { return reflexive_traffic_; }

  static void set_move_start(Time time) { move_start_ = time; }
  static Time get_move_start() { return move_start_; }

  static void set_move_end(Time time) { move_end_ = time; }
  static Time get_move_end() { return move_end_; }

  static void set_step_period(Time time) { step_period_ = time; }
  static Time get_step_period() { return step_period_; }

  static void set_min_speed(double speed) { min_speed_ = speed; }
  static double get_min_speed() { return min_speed_; }

  static void set_max_speed(double speed) { max_speed_ = speed; }
  static double get_max_speed() { return max_speed_; }

  static void set_min_pause(Time time) { min_pause_ = time; }
  static Time get_min_pause() { return min_pause_; }

  static void set_max_pause(Time time) { max_pause_ = time; }
  static Time get_max_pause() { return max_pause_; }

  static void set_routing_update_period(Time time) {
    routing_update_period_ = time;
  }
  static Time get_routing_update_period() { return routing_update_period_; }

  static bool DoPeriodicRoutingUpdate() {
    return static_cast<bool>(routing_update_period_);
  }

  static void set_routing_update_start(Time time) {
    routing_update_start_ = time;
  }
  static Time get_routing_update_start() { return routing_update_start_; }

  static void set_routing_update_end(Time time) { routing_update_end_ = time; }
  static Time get_routing_update_end() { return routing_update_end_; }

  static void set_neighbor_update_period(Time time) {
    neighbor_update_period_ = time;
  }
  static Time get_neighbor_update_period() { return neighbor_update_period_; }

  static void set_position_min(Position min) { position_min_ = min; }
  static Position get_position_min() { return position_min_; }

  static void set_position_max(Position max) { position_max_ = max; }
  static Position get_position_max() { return position_max_; }

  static std::size_t get_max_cube_side();

 private:
  // General
  static inline uint32_t node_count_ = 0;
  static inline Time duration_ = 0;
  static inline uint32_t ttl_limit_ = 0;
  static inline uint32_t connection_range_ = 0;

  // Traffic generation parameters.
  // TODO: 0 is reasonable so provide some default check
  static inline Time traffic_start_ = 0;
  static inline Time traffic_end_ = 0;
  static inline std::size_t traffic_event_count_ = 0;
  static inline bool reflexive_traffic_ = false;

  // Movement simulation parameters.
  static inline Time move_start_ = 0;
  static inline Time move_end_ = 0;
  static inline Time step_period_ = 0;
  static inline double min_speed_ = 0;  // m/s
  static inline double max_speed_ = 0;  // m/s
  static inline Time min_pause_ = 0;
  static inline Time max_pause_ = 0;

  // Periodic routing update parameters.
  static inline Time routing_update_period_ = 0;
  static inline Time routing_update_start_ = 0;
  static inline Time routing_update_end_ = 0;

  // Periodic neighbor update parameters.
  static inline Time neighbor_update_period_ = 0;

  // Position boundaries
  static inline Position position_min_ = Position(0, 0, 0);
  static inline Position position_max_ = Position(0, 0, 0);

  // MaxCube
  static inline bool is_position_cube_side_initialized_ = false;
  static inline uint32_t position_cube_side_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_PARAMETERS_H_
