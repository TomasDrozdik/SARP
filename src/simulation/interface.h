//
// interface.h
//

#ifndef SARP_SIMULATION_INTERFACE_H_
#define SARP_SIMULATION_INTERFACE_H_

#include <memory>
#include <stack>

#include "address.h"
#include "protocol_packet.h"
#include "routing.h"

namespace simulation {

class Interface {
 public:
  explicit Interface(Node &node, Node &other_end);

  void Send(std::unique_ptr<ProtocolPacket> &&packet) const;
  const Address& get_other_end_addr() const;

 private:
  Node &node_;
  Node &other_end_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_INTERFACE_H_