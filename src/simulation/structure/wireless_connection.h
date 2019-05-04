//
// wireless_connection.h
//

#ifndef SARP_SIMULATION_STRUCTURE_WIRELESS_CONNECTION_H_
#define SARP_SIMULATION_STRUCTURE_WIRELESS_CONNECTION_H_

#include "connection.h"

#include <cstdint>

namespace simulation {

class WirelessConnection final : public Connection {
 public:
  WirelessConnection(const Node &node, const Position position,
      uint32_t connection_range = 100);
  ~WirelessConnection() = default;

  std::vector<Node*> GetConnectedNodes(
      const std::vector<std::unique_ptr<Node>> &all_nodes) override;

  void set_connection_range(uint32_t connection_range);
 private:
  uint32_t connection_range_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
