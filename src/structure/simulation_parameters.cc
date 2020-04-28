//
// simulation.cc
//

#include "structure/simulation.h"

namespace simulation {

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
            << "\nposition_max: " << sp.position_max;
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
      routing_update_period(routing_update_period) {}

SimulationParameters::SimulationParameters(const SimulationParameters &other)
    : routing_type(other.routing_type),
      node_count(other.node_count),
      duration(other.duration),
      ttl_limit(other.ttl_limit),
      connection_range(other.connection_range),
      position_min(other.position_min),
      position_max(other.position_max),
      initial_positions_((other.initial_positions_)
                             ? other.initial_positions_->Clone()
                             : nullptr),
      has_traffic(other.has_traffic),
      has_movement(other.has_movement),
      has_periodic_routing_update(other.has_periodic_routing_update),
      traffic_start(other.traffic_start),
      traffic_end(other.traffic_end),
      traffic_event_count(other.traffic_event_count),
      move_start(other.move_start),
      move_end(other.move_end),
      move_step_period(other.move_step_period),
      move_speed_min(other.move_speed_min),
      move_speed_max(other.move_speed_max),
      move_pause_min(other.move_pause_min),
      move_pause_max(other.move_pause_max),
      neighbor_update_period(other.neighbor_update_period),
      move_directions_(
          (other.move_directions_) ? other.move_directions_->Clone() : nullptr),
      routing_update_period(other.routing_update_period) {}

SimulationParameters::SimulationParameters(SimulationParameters &&other)
    : SimulationParameters(other) {}

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

}  // namespace simulation
