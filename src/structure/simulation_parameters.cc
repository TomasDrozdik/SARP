//
// simulation_parameters.cc
//

#include "structure/simulation_parameters.h"

#include <algorithm>
#include <ostream>

namespace simulation {

std::ostream &SimulationParameters::Print(std::ostream &os) {
  return os << "___SIMULATION_PARAMETERS____"
            << "\nduration: " << duration_ << "\nttl_limit: " << ttl_limit_
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
            << "\nrouting_update_period: " << routing_update_period_
            << "\nrouting_update_start_: " << routing_update_start_
            << "\nrouting_update_end: " << routing_update_end_
            << "\nposition_min: " << position_min_
            << "\nposition_max: " << position_max_ << "\n\n_Routing_"
            << "\ndo_periodic_routing_update: "
            << SimulationParameters::DoPeriodicRoutingUpdate()
            << "\nperiodic_update_period: " << periodic_update_period_;
}

Time SimulationParameters::DeliveryDuration(
    const Node &from, const Node &to, const std::size_t packet_size_bytes) {
  // TODO: calculate properly
  uint32_t distance =
      Position::Distance(from.get_position(), to.get_position());
  Time t = distance / 10;
  t = std::min((Time)1, t);
  return t;
}

}  // namespace simulation
