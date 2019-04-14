//
// node.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NODE_H_
#define SARP_SIMULATION_STRUCTURE_NODE_H_

#include <memory>
#include <vector>

#include "address.h"
#include "connection.h"
#include "interface.h"
#include "network.h"
#include "position.h"
#include "routing.h"

namespace simulation {

class Address;
class Connection;
class Routing;
class ProtocolPacket;

class Node {
 public:
  Node(std::vector<std::unique_ptr<Address>> addresses,
      std::unique_ptr<Routing> routing,
      std::unique_ptr<Connection> connection);
  ~Node();

  void UpdateConnections(const std::vector<Node *> &all_nodes);
  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet);

  const int get_processing_power() const;
  const std::vector<std::unique_ptr<Address>> &get_addresses() const;
  const Connection& get_connection() const;

 private:
  std::vector<std::unique_ptr<Address>> addresses_;
  std::unique_ptr<Routing> routing_;
  std::unique_ptr<Connection> connection_;
  std::vector<Interface> active_connections_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NODE_H_
