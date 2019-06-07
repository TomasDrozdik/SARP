//
// wireless_connection.cc
//

#include "wireless_connection.h"

namespace simulation {

WirelessConnection::WirelessConnection(const Node &node,
    const Position position, uint32_t connection_range) :
        Connection(node, position),
        connection_range_(connection_range) { }

bool WirelessConnection::IsConnectedTo(const Node &node) const {
  uint32_t distance = Position::Distance(position,
      node.get_connection().position);
  return distance <= connection_range_;
}

void WirelessConnection::set_connection_range(uint32_t connection_range) {
  connection_range_ = connection_range;
}

}  // namespace simulation
