//
// simulation_parameters.cc
//

#include "structure/simulation_parameters.h"

#include <algorithm>
#include <memory>
#include <ostream>

#include "network_generator/position_generator.h"
#include "structure/position_cube.h"

// Exported global variable.
std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

static std::size_t get_max_cube_side(const Position &min, const Position &max) {
  uint32_t dx = std::abs(max.x - min.x);
  uint32_t dy = std::abs(max.y - min.y);
  uint32_t dz = std::abs(max.z - min.z);
  return std::max(dx, std::max(dy, dz));
}

SimulationParameters::SimulationParameters(
    RoutingType routing_type, uint32_t node_count, Time duration,
    uint32_t ttl_limit, uint32_t connection_range, Position position_min,
    Position position_max, std::unique_ptr<PositionGenerator> initial_positions,
    bool has_traffic, bool has_movement, bool has_periodic_routing_update,
    Time traffic_start, Time traffic_end, std::size_t traffic_event_count,
    Time move_start, Time move_end, Time move_step_period,
    double move_speed_min, double move_speed_max, Time move_pause_min,
    Time move_pause_max, Time neighbor_update_period,
    std::unique_ptr<PositionGenerator> move_directions,
    Time routing_update_period)
    : routing_type(routing_type),
      node_count(node_count),
      duration(duration),
      ttl_limit(ttl_limit),
      connection_range(connection_range),
      position_min(position_min),
      position_max(position_max),
      initial_positions_(std::move(initial_positions)),
      has_traffic(has_traffic),
      has_movement(has_movement),
      has_periodic_routing_update(has_periodic_routing_update),
      traffic_start(traffic_start),
      traffic_end(traffic_end),
      traffic_event_count(traffic_event_count),
      move_start(move_start),
      move_end(move_end),
      move_step_period(move_step_period),
      move_speed_min(move_speed_min),
      move_speed_max(move_speed_max),
      move_pause_min(move_pause_min),
      move_pause_max(move_pause_max),
      neighbor_update_period(neighbor_update_period),
      move_directions_(std::move(move_directions)),
      routing_update_period(routing_update_period),
      position_cube_max_side(get_max_cube_side(position_min, position_max)) {}

std::ostream &operator<<(std::ostream &os, const SimulationParameters &sp) {
  return os << "___SIMULATION_PARAMETERS____"
            << "\nnode_count: " << sp.node_count
            << "\nduration: " << sp.duration << "\nttl_limit: " << sp.ttl_limit
            << "\nconnection_range: " << sp.connection_range
            << "\ntraffic_start: " << sp.traffic_start
            << "\ntraffic_end: " << sp.traffic_end
            << "\ntraffic_event_count_: " << sp.traffic_event_count
            << "\n\n_Movement simulation sp.parameters_"
            << "\nmove_start: " << sp.move_start
            << "\nmove_end: " << sp.move_end
            << "\nstep_period: " << sp.move_step_period
            << "\nmin_speed: " << sp.move_speed_min << "m/sp.s"
            << "\nmax_speed: " << sp.move_speed_max << "m/sp.s"
            << "\nmin_pause: " << sp.move_pause_min
            << "\nmax_pause: " << sp.move_pause_max
            << "\nneighbor_update_period: " << sp.neighbor_update_period
            << "\n\n_Periodic routing update parameters_"
            << "\nrouting_update_period: " << sp.routing_update_period
            << "\nposition_min: " << sp.position_min
            << "\nposition_max: " << sp.position_max
            << "\nposition_cube_max_side: " << sp.position_cube_max_side;
}

Time SimulationParameters::DeliveryDuration(
    const Node &from, const Node &to, const std::size_t packet_size_bytes) {
  // TODO: calculate properly.
  uint32_t distance =
      Position::Distance(from.get_position(), to.get_position());
  Time t = distance / 10;
  if (t < 1) {
    t = 1;
  }
  return t;
}

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

std::unique_ptr<SimulationParameters> SimulationParametersBuilder::Build() {
  // HACK: If no movement is set neighbor update period needs to be set for
  // Routing::UpdateNeihbors to be called...
  if (!has_movement_ && has_periodic_routing_update_) {
    neighbor_update_period_ = routing_update_period_;
  }

  return std::make_unique<SimulationParameters>(
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
