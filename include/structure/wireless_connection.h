//
// wireless_connection.h
//

#ifndef SARP_STRUCTURE_WIRELESS_CONNECTION_H_
#define SARP_STRUCTURE_WIRELESS_CONNECTION_H_

#include "structure/connection.h"

#include <cstdint>

#include "structure/simulation.h"
namespace simulation {

template <int ConnectionRange = 100>
class WirelessConnection final : public Connection {
 public:
  WirelessConnection(const Node &node, const Position position,
      uint32_t connection_range = ConnectionRange) :
          Connection(node, position), connection_range_(connection_range) { }

  ~WirelessConnection() = default;

  bool IsConnectedTo(const Node &node) const {
    uint32_t distance = Position::Distance(position,
        node.get_connection().position);
    return distance <= connection_range_;
  }

  void set_connection_range(uint32_t connection_range) {
    connection_range_ = connection_range;
  }

 private:
  uint32_t connection_range_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_CONNECTION_H_
