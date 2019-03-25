//
// node.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NODE_H_
#define SARP_SIMULATION_STRUCTURE_NODE_H_

#include <memory>
#include <set>
#include <stack>

#include "address.h"
#include "connection.h"
#include "network.h"
#include "position.h"
#include "routing.h"

namespace simulation {

class Node {
 public:
  Node(std::set<std::unique_ptr<Address>> &addresses, Routing &routing,
      Connection &connection, const int process_power = 5);
  ~Node();

  void UpdateConnections(const std::set<Node const * const> &all_nodes);
  void Send(std::unique_ptr<ProtocolPacket> &&packet);
  void Recv(std::unique_ptr<ProtocolPacket> &&packet);
  void Process();

  const int get_processing_power() const;
  const std::set<std::unique_ptr<Address>>& get_addresses() const;
  const Connection& get_connection() const;

 private:
  const int process_power_;
  std::set<std::unique_ptr<Address>> addresses_;
  Routing &routing_;
  Connection &connection_;
  std::set<Interface> active_connections_;
  std::stack<std::unique_ptr<ProtocolPacket>> network_stack_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NODE_H_
