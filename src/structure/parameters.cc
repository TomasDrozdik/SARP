//
// simulation.cc
//

#include <iomanip>

#include "structure/simulation.h"

namespace simulation {

static void PrintRangeCsvHeader(std::ostream &os, std::string prefix) {
  os << prefix + "_min" << ',' << prefix + "_max" << ',';
}

static void PrintCostCsv(std::ostream &os, const Cost &cost) {
  // clang-format off
  os << cost.Mean() << ','
     << cost.Variance();
  // clang-format on
}
Parameters::General &Parameters::General::operator=(
    const Parameters::General &other) {
  routing_type = other.routing_type;
  node_count = other.node_count;
  duration = other.duration;
  ttl_limit = other.ttl_limit;
  connection_range = other.connection_range;
  neighbor_update_period = other.neighbor_update_period;
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
  directions = (other.directions) ? other.directions->Clone() : nullptr;
  return *this;
}

Parameters::Movement::Movement(const Parameters::Movement &other) {
  *this = other;
}



void Parameters::General::PrintCsvHeader(std::ostream &os) {
  // clang-format off
  os << "routing_type" << ','
     << "node_count" << ','
     << "time" << ','
     << "ttl_limit" << ','
     << "connection_range" << ','
     << "neighbor_update_period" << ',';
  // clang-format on
}

void Parameters::General::PrintCsv(std::ostream &os) const {
  // clang-format off
  os << routing_type << ','
     << node_count << ','
     << duration << ','
     << ttl_limit << ','
     << connection_range << ','
     << neighbor_update_period << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::General &p) {
  // clang-format off
  return os << "General:"
            << "\nrouting_type: " <<  p.routing_type
            << "\nnode_count: " << p.node_count
            << "\nduration: " << p.duration
            << "\nttl_limit: " << p.ttl_limit
            << "\nconnection_range: " << p.connection_range
            << "\nneighbor_update_period: " << p.neighbor_update_period
            << "\nposition_boundaries: " << p.position_boundaries;
  // clang-format on
}

void Parameters::Traffic::PrintCsvHeader(std::ostream &os) {
  PrintRangeCsvHeader(os, "traffíc_time");
  os << "event_count" << ',';
}

void Parameters::Traffic::PrintCsv(std::ostream &os) const {
  // clang-format off
  os << time_range << ','
     << event_count << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::Traffic &p) {
  // clang-format off
  return os << "Traffic parameters:"
            << "\ntraffic_time_range: " << p.time_range
            << "\ntraffic_event_count_: " << p.event_count;
  // clang-format on
}

void Parameters::Movement::PrintCsvHeader(std::ostream &os) {
  PrintRangeCsvHeader(os, "move_time");
  os << "step_period" << ',';
  PrintRangeCsvHeader(os, "speed");
  PrintRangeCsvHeader(os, "pause");
}

void Parameters::Movement::PrintCsv(std::ostream &os) const {
  // clang-format off
  os << time_range << ','
     << step_period << ','
     << speed_range << ','
     << pause_range << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p) {
  // clang-format off
  return os << "Movement parameters:"
            << "\nmove_time_interval: " << p.time_range
            << "\nstep_period: " << p.step_period
            << "\nspeed_range: " << p.speed_range << "m/sp.s"
            << "\npause_interval: " << p.pause_range;
  // clang-format on
}

void Parameters::PeriodicRouting::PrintCsvHeader(std::ostream &os) {
  os << "routing_update_period" << ',';
}

void Parameters::PeriodicRouting::PrintCsv(std::ostream &os) const {
  os << update_period << ',';
}

std::ostream &operator<<(std::ostream &os,
                         const Parameters::PeriodicRouting &p) {
  // clang-format off
  return os << "Periodic update parameters:"
            << "\nrouting_update_period: " << p.update_period;
  // clang-format on
}

void Parameters::Sarp::PrintCsvHeader(std::ostream &os) {
  // clang-format off
  os << "neighbor_mean" << ','
     << "neighbor_var" << ','
     << "neighbor_sd" << ','
     << "reflexive_cost" << ','
     << "reflexive_var" << ','
     << "reflexive_sd" << ','
     << "compact_treshold" << ','
     << "update_treshold" << ',';
  // clang-format on
}

void Parameters::Sarp::PrintCsv(std::ostream &os) const {
  // clang-format off
  PrintCostCsv(os, neighbor_cost);
  PrintCostCsv(os, reflexive_cost);
  PrintCostCsv(os, max_cost);
  os << compact_treshold << ','
     << update_treshold << ','
     << min_standard_deviation << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p) {
  // clang-format off
  return os << "Sarp paramters:"
            << "\nneighbor_cost: " << p.neighbor_cost
            << "\nreflexive_cost: " << p.reflexive_cost
            << "\nmax_cost: " << p.max_cost
            << "\ncompact_treshold: " << p.compact_treshold
            << "\nupdate_treshold: " << p.update_treshold
            << "\nmin_standard_deviation: " << p.min_standard_deviation;;
  // clang-format on
}

void Parameters::PrintCsvHeader(std::ostream &os) {
  os << "has_general" << ',';
  Parameters::General::PrintCsvHeader(os);
  os << "has_traffic" << ',';
  Parameters::Traffic::PrintCsvHeader(os);
  os << "has_movement" << ',';
  Parameters::Movement::PrintCsvHeader(os);
  os << "has_periodic_routing" << ',';
  Parameters::PeriodicRouting::PrintCsvHeader(os);
  os << "has_sarp_parameters" << ',';
  Parameters::Sarp::PrintCsvHeader(os);
}

void Parameters::PrintCsv(std::ostream &os) const {
  os << general_.first << ',';
  general_.second.PrintCsv(os);
  os << traffic_.first << ',';
  traffic_.second.PrintCsv(os);
  os << movement_.first << ',';
  movement_.second.PrintCsv(os);
  os << periodic_routing_.first << ',';
  periodic_routing_.second.PrintCsv(os);
  os << sarp_parameters_.first << ',';
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
