//
// simulation_parameters.cc
//

#include "structure/simulation_parameters.h"

#include <algorithm>
#include <ostream>

#include "structure/position_cube.h"

namespace simulation {

std::ostream &SimulationParameters::Print(std::ostream &os) {
  return os << "___SIMULATION_PARAMETERS____"
            << "\nnode_count: " << node_count_ << "\nduration: " << duration_
            << "\nttl_limit: " << ttl_limit_
            << "\nconnection_range: " << connection_range_
            << "\ntraffic_start: " << traffic_start_
            << "\ntraffic_end: " << traffic_end_
            << "\ntraffic_event_count_: " << traffic_event_count_
            << "\nreflexive_traffic: " << reflexive_traffic_
            << "\n\n_Movement simulation parameters_"
            << "\nmove_start: " << move_start_ << "\nmove_end: " << move_end_
            << "\nstep_period: " << step_period_
            << "\nmin_speed: " << min_speed_ << "m/s"
            << "\nmax_speed: " << max_speed_ << "m/s"
            << "\nmin_pause: " << min_pause_ << "\nmax_pause: " << max_pause_
            << "\n\n_Periodic routing update parameters_"
            << "\ndo_periodic_routing_update: "
            << SimulationParameters::DoPeriodicRoutingUpdate()
            << "\nrouting_update_period: " << routing_update_period_
            << "\nrouting_update_start_: " << routing_update_start_
            << "\nrouting_update_end: " << routing_update_end_
            << "\nposition_min: " << position_min_
            << "\nposition_max: " << position_max_;
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

std::size_t SimulationParameters::get_max_cube_side() {
  if (is_position_cube_side_initialized_) {
    return position_cube_side_;
  }
  unsigned dx = std::abs(position_max_.x - position_min_.x);
  unsigned dy = std::abs(position_max_.y - position_min_.y);
  unsigned dz = std::abs(position_max_.z - position_min_.z);
  position_cube_side_ = std::max(dx, std::max(dy, dz));
  return position_cube_side_;
}

}  // namespace simulation
