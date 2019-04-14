//
// wireless_connection.h
//

#ifndef SARP_SIMULATION_STRUCTURE_WIRELESS_CONNECTION_H_
#define SARP_SIMULATION_STRUCTURE_WIRELESS_CONNECTION_H_

#include "connection.h"

namespace simulation {

class WirelessConnection : Connection {
 public:
  WirelessConnection(const Node &node, const Position position,
      int connection_range = 100);
  ~WirelessConnection() = default;

  std::unique_ptr<std::vector<Node*>> GetConnectedNodes(
      const std::vector<Node*> &all_nodes) override;

 private:
  Position position_;
  const int connection_range_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
