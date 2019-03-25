//
// wireless_connection.cc
//

#include "wireless_connection.h"

#include <cassert>

namespace simulation {

WirelessConnection::WirelessConnection(const Node &node,
    const Position position, int connection_range) :
    Connection(node),
    position_(position),
    connection_range_(connection_range) { }

std::unique_ptr<std::set<Node const * const >>&& WirelessConnection::GetConnectedNodes(
    const std::set<Node const * const> &all_nodes) {
  auto active_connections = std::make_unique<std::set<Node const * const>>();
  for (auto node : all_nodes) {
    int distance = Position::Distance(position_,
        dynamic_cast<WirelessConnection&>(
            const_cast<Connection&>(node->get_connection())).position_);
    // Check if the distance is lower than range of the connection
    if (connection_range_ <= distance) {
      auto ret = active_connections->insert(node);
      // Assert that emplace was successful
      assert(ret.second);
    }
  }
  return std::move(active_connections);
}

}  // namespace simulation
