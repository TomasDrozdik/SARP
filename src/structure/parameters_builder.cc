//
// parameters_builder.cc
//

#include "structure/parameters_builder.h"

#include <algorithm>
#include <memory>
#include <ostream>

#include "network_generator/position_generator.h"
#include "structure/position_cube.h"

namespace simulation {

SimulationParametersBuilder::SimulationParametersBuilder(
    RoutingType routing_type, uint32_t node_count, Time duration,
    uint32_t ttl_limit, uint32_t connection_range, Position position_min,
    Position position_max, std::unique_ptr<PositionGenerator> initial_positions)
    : routing_type_(routing_type),
      node_count_(node_count),
      duration_(duration),
      ttl_limit_(ttl_limit),
      connection_range_(connection_range),
      position_min_(position_min),
      position_max_(position_max),
      initial_positions_(std::move(initial_positions)) {}

SimulationParametersBuilder &SimulationParametersBuilder::AddTraffic(
    Time start, Time end, std::size_t event_count) {
  has_traffic_ = true;
  traffic_start_ = start;
  traffic_end_ = end;
  traffic_event_count_ = event_count;
  return *this;
}

SimulationParametersBuilder &SimulationParametersBuilder::AddRandomMovement(
    Time start, Time end, Time step_period, double speed_min, double speed_max,
    Time pause_min, Time pause_max, Time neighbor_update_period) {
  has_movement_ = true;
  move_start_ = start;
  move_end_ = end;
  move_step_period_ = step_period;
  move_speed_min_ = speed_min;
  move_speed_max_ = speed_max;
  move_pause_min_ = pause_min;
  move_pause_max_ = pause_max;
  neighbor_update_period_ = neighbor_update_period;
  move_directions_ =
      std::make_unique<RandomPositionGenerator>(position_min_, position_max_);
  return *this;
}

SimulationParametersBuilder &
SimulationParametersBuilder::AddPeriodicRoutingUpdate(Time update_period) {
  has_periodic_routing_update_ = true;
  routing_update_period_ = update_period;
  return *this;
}

SimulationParameters SimulationParametersBuilder::Build() {
  // HACK: If no movement is set neighbor update period needs to be set for
  // Routing::UpdateNeihbors to be called...
  if (!has_movement_ && has_periodic_routing_update_) {
    neighbor_update_period_ = routing_update_period_;
  }

  return SimulationParameters(
      routing_type_, node_count_, duration_, ttl_limit_, connection_range_,
      position_min_, position_max_, initial_positions_->Clone(), has_traffic_,
      has_movement_, has_periodic_routing_update_, traffic_start_, traffic_end_,
      traffic_event_count_, move_start_, move_end_, move_step_period_,
      move_speed_min_, move_speed_max_, move_pause_min_, move_pause_max_,
      neighbor_update_period_,
      (move_directions_ ? move_directions_->Clone() : nullptr),
      routing_update_period_);
}

}  // namespace simulation
