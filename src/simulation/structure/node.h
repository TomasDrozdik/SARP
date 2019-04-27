//
// node.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NODE_H_
#define SARP_SIMULATION_STRUCTURE_NODE_H_

#include <exception>
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
  Node();
  ~Node() = default;

  void UpdateConnections(const std::vector<std::unique_ptr<Node>> &all_nodes);
  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet);
  bool IsInitialized() const;
  void Print() const;

  void add_address(std::unique_ptr<Address> addr);
  void set_addresses(std::vector<std::unique_ptr<Address>> addresses);
  void set_connection(std::unique_ptr<Connection> connection);
  void set_routing(std::unique_ptr<Routing> routing);

  std::vector<Interface>& get_active_connections();
  const std::vector<std::unique_ptr<Address>>& get_addresses() const;
  const Connection& get_connection() const;
  Routing& get_routing();

 private:
  std::vector<std::unique_ptr<Address>> addresses_;
  std::vector<Interface> active_connections_;
  std::unique_ptr<Connection> connection_;
  std::unique_ptr<Routing> routing_;
};

class UninitializedException : std::exception {
 public:
  const char* what() const noexcept override {
    return "Use of yet uninitialized instance of Node.";
  }
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NODE_H_
