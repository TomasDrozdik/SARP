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

void Parameters::General::PrintCsvHeader(std::ostream &os) {
  // clang-format off
  os << "duration" << ','
     << "ttl_limit" << ','
     << "connection_range" << ','
     << "routing_update_period" << ','
     << "neighbor_update_period" << ',';
  // clang-format on
}

void Parameters::General::PrintCsv(std::ostream &os) const {
  // clang-format off
  os << duration << ','
     << ttl_limit << ','
     << connection_range << ','
     << routing_update_period << ','
     << neighbor_update_period << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::General &p) {
  // clang-format off
  return os << "General:"
            << "\nduration: " << p.duration
            << "\nttl_limit: " << p.ttl_limit
            << "\nconnection_range: " << p.connection_range
            << "\nrouting_update_period: " << p.neighbor_update_period
            << "\nneighbor_update_period: " << p.neighbor_update_period
            << "\nboundaries: " << p.boundaries;
  // clang-format on
}

void Parameters::NodeGeneration::PrintCsvHeader(std::ostream &os) {
  os << "node_count" << ','
     << "routing_type" << ',';
}

void Parameters::NodeGeneration::PrintCsv(std::ostream &os) const {
  os << node_count << ','
     << routing_type << ',';
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
  // clang-format off
  os << "move_end" << ','
     << "step_period" << ',';
  PrintRangeCsvHeader(os, "speed");
  PrintRangeCsvHeader(os, "pause");
  // clang-format on
}

void Parameters::Movement::PrintCsv(std::ostream &os) const {
  // clang-format off
  os << end << ','
     << step_period << ','
     << speed_range << ','
     << pause_range << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::Movement &p) {
  // clang-format off
  return os << "Movement parameters:"
            << "\nmove_end: " << p.end
            << "\nstep_period: " << p.step_period
            << "\nspeed_range: " << p.speed_range << "m/sp.s"
            << "\npause_interval: " << p.pause_range;
  // clang-format on
}

void Parameters::Sarp::PrintCsvHeader(std::ostream &os) {
  // clang-format off
  os << "neighbor_mean" << ','
     << "neighbor_var" << ','
     << "neighbor_sd" << ','
     << "compact_treshold" << ','
     << "update_treshold" << ','
     << "min_standard_deviation" << ',';
  // clang-format on
}

void Parameters::Sarp::PrintCsv(std::ostream &os) const {
  // clang-format off
  PrintCostCsv(os, neighbor_cost);
  os << compact_treshold << ','
     << update_treshold << ','
     << min_standard_deviation << ',';
  // clang-format on
}

std::ostream &operator<<(std::ostream &os, const Parameters::Sarp &p) {
  // clang-format off
  return os << "Sarp paramters:"
            << "\nneighbor_cost: " << p.neighbor_cost
            << "\ncompact_treshold: " << p.compact_treshold
            << "\nupdate_treshold: " << p.update_treshold
            << "\nmin_standard_deviation: " << p.min_standard_deviation;
  // clang-format on
}

void Parameters::PrintCsvHeader(std::ostream &os) {
  os << "has_general" << ',';
  Parameters::General::PrintCsvHeader(os);
  os << "has_node_generation" << ',';
  Parameters::NodeGeneration::PrintCsvHeader(os);
  os << "has_traffic" << ',';
  Parameters::Traffic::PrintCsvHeader(os);
  os << "has_movement" << ',';
  Parameters::Movement::PrintCsvHeader(os);
  os << "has_sarp" << ',';
  Parameters::Sarp::PrintCsvHeader(os);
}

void Parameters::PrintCsv(std::ostream &os) const {
  os << has_general() << ',';
  general_.second.PrintCsv(os);
  os << has_node_generation() << ',';
  node_generation_.second.PrintCsv(os);
  os << has_traffic() << ',';
  traffic_.second.PrintCsv(os);
  os << has_movement() << ',';
  movement_.second.PrintCsv(os);
  os << has_sarp() << ',';
  sarp_parameters_.second.PrintCsv(os);
}

std::ostream &operator<<(std::ostream &os, const Parameters &p) {
  os << "SIMULATION PARAMETERS\n";
  if (p.has_general()) {
    os << p.general_.second << "\n\n";
  }
  if (p.has_node_generation()) {
    os << p.general_.second << "\n\n";
  }
  if (p.has_traffic()) {
    os << p.traffic_.second << "\n\n";
  }
  if (p.has_movement()) {
    os << p.movement_.second << "\n\n";
  }
  if (p.has_sarp()) {
    os << p.sarp_parameters_.second << "\n\n";
  }
  return os;
}

}  // namespace simulation
