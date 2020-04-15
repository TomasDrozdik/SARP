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
class SimulationParameters;

std::ostream &operator<<(std::ostream &os, const SimulationParameters &sp);

enum class RoutingType {
  STATIC,
  DISTANCE_VECTOR,
  SARP,
};

struct SimulationParameters final {
  friend class SimulationParametersBuilder;
  // TODO: Create a builder for this one.
  SimulationParameters(
      RoutingType routing_type, uint32_t node_count, Time duration,
      uint32_t ttl_limit, uint32_t connection_range, Position position_min,
      Position position_max,
      std::unique_ptr<PositionGenerator> initial_positions,
      bool has_traffic = false, bool has_movement = false,
      bool has_periodic_routing_update = false, Time traffic_start = 0,
      Time traffic_end = 0, std::size_t traffic_event_count = 0,
      Time move_start = 0, Time move_end = 0, Time move_step_period = 0,
      double move_speed_min = 0, double move_speed_max = 0,
      Time move_pause_min = 0, Time move_pause_max = 0,
      Time neighbor_update_period = 0,
      std::unique_ptr<PositionGenerator> move_directions = nullptr,
      Time routing_update_period = 0);

  SimulationParameters(const SimulationParameters &other);

  static Time DeliveryDuration(const Node &from, const Node &to,
                               const std::size_t packet_size);

  std::unique_ptr<PositionGenerator> get_initial_positions() const {
    return initial_positions_->Clone();
  }

  std::unique_ptr<PositionGenerator> get_move_directions() const {
    return move_directions_->Clone();
  }

  // General
  const RoutingType routing_type;
  const uint32_t node_count;
  const Time duration;
  const uint32_t ttl_limit;
  const uint32_t connection_range;
  const Position position_min;
  const Position position_max;
  const std::unique_ptr<PositionGenerator> initial_positions_;

  const bool has_traffic;
  const bool has_movement;
  const bool has_periodic_routing_update;

  // Traffic generation parameters.
  const Time traffic_start;
  const Time traffic_end;
  const std::size_t traffic_event_count;

  // Movement simulation parameters.
  const Time move_start;
  const Time move_end;
  const Time move_step_period;
  const double move_speed_min;  // m/s
  const double move_speed_max;  // m/s
  const Time move_pause_min;
  const Time move_pause_max;
  // Compute these positions from initial positions if not provided.
  const Time neighbor_update_period;
  const std::unique_ptr<PositionGenerator> move_directions_;

  // Periodic routing update parameters.
  const Time routing_update_period;

  // Precalculated parameters based on either provided or computed position
  // boundaries.
  const uint32_t position_cube_max_side;
};

class SimulationParametersBuilder {
 public:
  SimulationParametersBuilder(
      RoutingType routing_type, uint32_t node_count, Time duration,
      uint32_t ttl_limit, uint32_t connection_range, Position position_min,
      Position position_max,
      std::unique_ptr<PositionGenerator> initial_positions);

  SimulationParametersBuilder &AddTraffic(Time start, Time end,
                                          std::size_t event_count);

  SimulationParametersBuilder &AddRandomMovement(Time start, Time end,
                                                 Time step_period,
                                                 double speed_min,
                                                 double speed_max,
                                                 Time pause_min, Time pause_max,
                                                 Time neighbor_update_period);

  SimulationParametersBuilder &AddPeriodicRoutingUpdate(Time update_period);

  std::unique_ptr<SimulationParameters> Build();

 private:
  // General
  RoutingType routing_type_;
  uint32_t node_count_;
  Time duration_;
  uint32_t ttl_limit_;
  uint32_t connection_range_;
  Position position_min_;
  Position position_max_;
  std::unique_ptr<PositionGenerator> initial_positions_;

  // Traffic generation parameters.
  bool has_traffic_ = false;
  Time traffic_start_ = 0;
  Time traffic_end_ = 0;
  std::size_t traffic_event_count_ = 0;

  // Movement simulation parameters.
  bool has_movement_ = false;
  Time move_start_ = 0;
  Time move_end_ = 0;
  Time move_step_period_ = 0;
  double move_speed_min_ = 0;  // m/s
  double move_speed_max_ = 0;  // m/s
  Time move_pause_min_ = 0;
  Time move_pause_max_ = 0;
  Time neighbor_update_period_ = 0;
  std::unique_ptr<PositionGenerator> move_directions_ = 0;

  // Periodic routing update parameters.
  bool has_periodic_routing_update_ = false;
  Time routing_update_period_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_PARAMETERS_H_
