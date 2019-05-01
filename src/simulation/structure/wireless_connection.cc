//
// wireless_connection.cc
//

#include "wireless_connection.h"

namespace simulation {

WirelessConnection::WirelessConnection(const Node &node,
    const Position position, uint32_t connection_range) :
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
    uint32_t distance = Position::Distance(position,
        all_nodes[i]->get_connection().position);
    // Check if the distance is lower than range of the connection
    if (distance <= connection_range_) {
      active_connections.push_back(all_nodes[i].get());
    }
  }
  return active_connections;
}

void WirelessConnection::set_connection_range(uint32_t connection_range) {
  connection_range_ = connection_range;
}

}  // namespace simulation
