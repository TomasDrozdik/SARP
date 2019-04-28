//
// wireless_connection.cc
//

#include "wireless_connection.h"

namespace simulation {

WirelessConnection::WirelessConnection(const Node &node,
    const Position position, int connection_range) :
        Connection(node, position),
        connection_range_(connection_range) { }

std::vector<Node*> WirelessConnection::GetConnectedNodes(
    const std::vector<std::unique_ptr<Node>> &all_nodes) {
  std::vector<Node*> active_connections;
  for (std::size_t i = 0; i < all_nodes.size(); ++i) {
    // Check for the same node.
    if (all_nodes[i].get() == &node_) {
      continue;
    }
    int distance = Position::Distance(position_,
        all_nodes[i]->get_connection().position_);
    // Check if the distance is lower than range of the connection
    if (distance <= connection_range_) {
      active_connections.push_back(all_nodes[i].get());
    }
  }
  return active_connections;
}

}  // namespace simulation
