//
// simulation.cc
//

#include "structure/simulation.h"

#include <iomanip>

namespace simulation {

static void PrintRangeCsvHeader(std::ostream &os, std::string prefix) {
  os << prefix + "_min" << ','
     << prefix + "_max" << ',';
}

static void PrintCostCsv(std::ostream &os, const Cost &cost) {
  os << std::setw(W) << cost.mean << ','
     << std::setw(W) << cost.sd << ','
     << std::setw(W) << cost.group_size << ',';
}

std::ostream &operator<<(std::ostream &os, const RoutingType &r) {
  switch (r) {
    case RoutingType::STATIC:
      os << std::setw(W) << "STATIC";
      break;
    case RoutingType::DISTANCE_VECTOR:
      os << std::setw(W) << "DISTANCE_VECTOR";
      break;
    case RoutingType::SARP:
      os << std::setw(W) << "SARP";
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
  return os << "{.mean = " << cost.mean << " .sd = " << cost.sd
            << " .group_size = " << cost.group_size << '}';
}

void Parameters::General::PrintCsvHeader(std::ostream &os) const {
  os << std::setw(W) << "routing_type" << ','
     << std::setw(W) << "node_count" << ','
     << std::setw(W) << "time" << ','
     << std::setw(W) << "ttl_limit" << ','
     << std::setw(W) << "connection_range" << ',';
}

void Parameters::General::PrintCsv(std::ostream &os) const {
  os << std::setw(W) << routing_type << ','
     << std::setw(W) << node_count << ','
     << std::setw(W) << duration << ','
     << std::setw(W) << ttl_limit << ','
     << std::setw(W) << connection_range << ',';
     // Don't print these since they will mess up csv header
     //<< std::setw(W) << p.position_boundaries << ',';
}

std::ostream &operator<<(std::ostream &os, const Parameters::General &p) {
  return os << "General:"
            << "\nrouting_type: " <<  p.routing_type
            << "\nnode_count: " << p.node_count
            << "\nduration: " << p.duration
            << "\nttl_limit: " << p.ttl_limit
            << "\nconnection_range: " << p.connection_range
            << "\nposition_boundaries: " << p.position_boundaries;
}

void Parameters::Traffic::PrintCsvHeader(std::ostream &os) const {
  PrintRangeCsvHeader(os, "traffíc_time");
  os << std::setw(W) << "event_count" << ',';
}

void Parameters::Traffic::PrintCsv(std::ostream &os) const {
  os << std::setw(W) << time_range << ','
     << std::setw(W) << event_count << ',';
}

std::ostream &operator<<(std::ostream &os, const Parameters::Traffic &p) {
  return os << "Traffic parameters:"
            << "\ntraffic_time_range: " << p.time_range
            << "\ntraffic_event_count_: " << p.event_count;
}

void Parameters::Movement::PrintCsvHeader(std::ostream &os) const {
  PrintRangeCsvHeader(os, "move_time");
  os << std::setw(W) << "step_period" << ',';
  PrintRangeCsvHeader(os, "speed");
  PrintRangeCsvHeader(os, "pause");
  os << std::setw(W) << "neighbor_update_period" << ',';
}

void Parameters::Movement::PrintCsv(std::ostream &os) const {
  os << std::setw(W) << time_range << ','
     << std::setw(W) << step_period << ','
     << std::setw(W) << speed_range << ','
     << std::setw(W) << pause_range << ','
     << std::setw(W) << neighbor_update_period << ',';
}

std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p) {
  return os << "Movement parameters:"
            << "\nmove_time_interval: " << p.time_range
            << "\nstep_period: " << p.step_period
            << "\nspeed_range: " << p.speed_range << "m/sp.s"
            << "\npause_interval: " << p.pause_range
            << "\nneighbor_update_period: " << p.neighbor_update_period;
}

void Parameters::PeriodicRouting::PrintCsvHeader(std::ostream &os) const {
  os << std::setw(W) << "routing_update_period" << ',';
}

void Parameters::PeriodicRouting::PrintCsv(std::ostream &os) const {
  os << std::setw(W) << update_period << ',';
}

std::ostream &operator<<(std::ostream &os, const Parameters::PeriodicRouting &p) {
  return os << "Periodic update parameters:"
            << "\nrouting_update_period: " << p.update_period;
}

void Parameters::Sarp::PrintCsvHeader(std::ostream &os) const {
  os << std::setw(W) << "neighbor_mean" << ','
     << std::setw(W) << "neighbor_sd" << ','
     << std::setw(W) << "neighbor_group_size" << ','
     << std::setw(W) << "reflexive_cost" << ','
     << std::setw(W) << "reflexive_sd" << ','
     << std::setw(W) << "reflexive_group_size" << ','
     << std::setw(W) << "treshold" << ',';
}

void Parameters::Sarp::PrintCsv(std::ostream &os) const {
  os << std::setw(W);
  PrintCostCsv(os, neighbor_cost);
  os << std::setw(W);
  PrintCostCsv(os, reflexive_cost);
  os << std::setw(W) << treshold << ',';
}

std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p) {
  return os << "Sarp paramters:"
            << "\nneighbor_cost: " << p.neighbor_cost
            << "\nreflexive_cost: " << p.reflexive_cost
            << "\nquantile_treshold: " << p.treshold;
}

void Parameters::PrintCsvHeader(std::ostream &os) const {
  os << std::setw(W) << "has_general" << ',';
  general_.second.PrintCsvHeader(os);
  os << std::setw(W) << "has_traffic" << ',';
  traffic_.second.PrintCsvHeader(os);
  os << std::setw(W) << "has_movement" << ',';
  movement_.second.PrintCsvHeader(os);
  os << std::setw(W) << "has_periodic_routing" << ',';
  periodic_routing_.second.PrintCsvHeader(os);
  os << std::setw(W) << "has_sarp_parameters" << ',';
  sarp_parameters_.second.PrintCsvHeader(os);
}

void Parameters::PrintCsv(std::ostream &os) const {
  os << std::setw(W) << general_.first << ',';
  general_.second.PrintCsv(os);
  os << std::setw(W) << traffic_.first << ',';
  traffic_.second.PrintCsv(os);
  os << std::setw(W) << movement_.first << ',';
  movement_.second.PrintCsv(os);
  os << std::setw(W) << periodic_routing_.first << ',';
  periodic_routing_.second.PrintCsv(os);
  os << std::setw(W) << sarp_parameters_.first << ',';
  sarp_parameters_.second.PrintCsv(os);
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
