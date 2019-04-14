//
// wireless_connection.cc
//

#include "wireless_connection.h"

namespace simulation {

WirelessConnection::WirelessConnection(const Node &node,
    const Position position, int connection_range) :
        Connection(node),
        position_(position),
        connection_range_(connection_range) { }

std::unique_ptr<std::vector<Node*>> WirelessConnection::GetConnectedNodes(
    const std::vector<Node*> &all_nodes) {
  auto active_connections = std::make_unique<std::vector<Node*>>();
  for (auto node : all_nodes) {
    int distance = Position::Distance(position_,
        dynamic_cast<WirelessConnection&>(
            const_cast<Connection&>(node->get_connection())).position_);
    // Check if the distance is lower than range of the connection
    if (connection_range_ <= distance) {
      active_connections->push_back(node);
    }
  }
  return std::move(active_connections);
}

}  // namespace simulation
