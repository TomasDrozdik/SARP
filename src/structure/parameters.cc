//
// simulation.cc
//

#include "structure/simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const RoutingType &r) {
  switch (r) {
    case RoutingType::STATIC:
      os << "STATIC";
      break;
    case RoutingType::DISTANCE_VECTOR:
      os << "DISTANCE_VECTOR";
      break;
    case RoutingType::SARP:
      os << "SARP";
      break;
    default:
      assert(false);
  }
  return os;
}

Parameters::General &Parameters::General::operator=(
    const Parameters::General &other) {
  routing_type = other.routing_type;
  node_count = other.node_count;
  duration = other.duration;
  ttl_limit = other.ttl_limit;
  connection_range = other.connection_range;
  position_boundaries = other.position_boundaries;
  initial_addresses =
      (other.initial_addresses) ? other.initial_addresses->Clone() : nullptr;
  initial_positions =
      (other.initial_positions) ? other.initial_positions->Clone() : nullptr;
  return *this;
}

Parameters::General::General(const Parameters::General &other) {
  *this = other;
}

Parameters::Movement &Parameters::Movement::operator=(
    const Parameters::Movement &other) {
  time_range = other.time_range;
  step_period = other.step_period;
  speed_range = other.speed_range;
  pause_range = other.pause_range;
  neighbor_update_period = other.neighbor_update_period;
  directions = (other.directions) ? other.directions->Clone() : nullptr;
  return *this;
}

Parameters::Movement::Movement(const Parameters::Movement &other) {
  *this = other;
}

std::ostream &operator<<(std::ostream &os, const Cost &cost) {
  return os << "{.mean = " << cost.mean << " .sp = " << cost.sd
            << " .group_size = " << cost.group_size << '}';
}

std::ostream &operator<<(std::ostream &os, const Parameters::General &p) {
  return os << "General parameters:"
            << "\nrouting_type: " << p.routing_type
            << "\nnode_count: " << p.node_count << "\nduration: " << p.duration
            << "\nttl_limit: " << p.ttl_limit
            << "\nconnection_range: " << p.connection_range
            << "\nposition_boundaries: " << p.position_boundaries;
}

std::ostream &operator<<(std::ostream &os, const Parameters::Traffic &p) {
  return os << "Traffic parameters:"
            << "\ntraffic_time_range: " << p.time_range
            << "\ntraffic_event_count_: " << p.event_count;
}

std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p) {
  return os << "Movement parameters:"
            << "\nmove_time_interval: " << p.time_range
            << "\nstep_period: " << p.step_period
            << "\nspeed_range: " << p.speed_range << "m/sp.s"
            << "\npause_interval: " << p.pause_range
            << "\nneighbor_update_period: " << p.neighbor_update_period;
}

std::ostream &operator<<(std::ostream &os,
                         const Parameters::PeriodicRouting &p) {
  return os << "Periodic update parameters:"
            << "\nrouting_update_period: " << p.update_period;
}

std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p) {
  return os << "Sarp paramters:"
            << "\nneighbor_cost: " << p.neighbor_cost
            << "\nreflexive_cost: " << p.reflexive_cost
            << "\nquantile_treshold: " << p.treshold
            << "\ndo_compacting: " << p.do_compacting;
}

std::ostream &operator<<(std::ostream &os, const Parameters &p) {
  os << "SIMULATION PARAMETERS\n";
  if (p.general_.first) {
    os << p.general_.second << "\n\n";
  }
  if (p.traffic_.first) {
    os << p.traffic_.second << "\n\n";
  }
  if (p.movement_.first) {
    os << p.movement_.second << "\n\n";
  }
  if (p.periodic_routing_.first) {
    os << p.periodic_routing_.second << "\n\n";
  }
  if (p.sarp_parameters_.first) {
    os << p.sarp_parameters_.second << "\n\n";
  }
  return os;
}

}  // namespace simulation
