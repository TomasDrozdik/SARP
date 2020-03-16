//
// wireless_connection.h
//

#ifndef SARP_STRUCTURE_WIRELESS_CONNECTION_H_
#define SARP_STRUCTURE_WIRELESS_CONNECTION_H_

#include <cstdint>

#include "structure/connection.h"
#include "structure/simulation.h"

namespace simulation {

class WirelessConnection final : public Connection {
 public:
  WirelessConnection(const Node &node, const Position position)
      : Connection(node, position) {}

  ~WirelessConnection() = default;

  bool IsConnectedTo(const Node &node) const {
    uint32_t distance =
        Position::Distance(position, node.get_connection().position);
    return distance <= Simulation::get_instance()
                           .get_simulation_parameters()
                           .get_connection_range();
  }
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_CONNECTION_H_
