//
// node.h
//

#ifndef SARP_SIMULATION_NODE_H_
#define SARP_SIMULATION_NODE_H_

#include <memory>
#include <set>
#include <stack>

#include "address.h"
#include "position.h"
#include "routing.h"

namespace simulation {

class Node {
 public:
  Node(const Position position, const Address &addr, const Routing &routing,
      int connection_range = 100, int processing_power = 5);
  ~Node();

  void UpdateConnections(std::set<const Node&> &all_node_connections);
  void Send(std::unique_ptr<ProtocolPacket> &&packet);
  void Recv(std::unique_ptr<ProtocolPacket> &&packet);
  void Process();

  const Address& get_address() const;
  const int get_processing_power() const;

 private:
  const int connection_range_;
  const int node_pocess_power_factor_;
  Position position_;
  Address &addr_;
  Routing &routing_table_;
  std::set<Interface> active_connections_;
  std::stack<std::unique_ptr<ProtocolPacket>> network_stack_;
};



}  // namespace simulation

#endif  // SARP_SIMULATION_NODE_H_
