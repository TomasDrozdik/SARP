//
// simulation_parameters.cc
//

#include "structure/simulation_parameters.h"

#include <ostream>

namespace simulation {

std::ostream &SimulationParameters::Print(std::ostream &os) {
  return os << "___SIMULATION_PARAMETERS____\n" <<
      "duration:" << duration_ << '\n' <<
      "ttl_limit::" << ttl_limit_ << '\n' <<
      "connection_range::" << connection_range_ << '\n';
}

Time SimulationParameters::DeliveryDuration(
    const Node &from, const Node &to, const std::size_t packet_size_bytes) {
  // TODO: calculate properly
  uint32_t distance =
      Position::Distance(from.get_position(), to.get_position());
  return 5;
}

}  // namespace simulation

