//
// node.h
//

#ifndef SARP_STRUCTURE_NODE_H_
#define SARP_STRUCTURE_NODE_H_

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
class Interface;
class ProtocolPacket;

class Node {
 friend std::ostream &operator<<(std::ostream &os, const Node &node);
 public:
  Node();
  ~Node() = default;

  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet,
      Interface *processing_interface);
  bool IsInitialized() const;

  void add_address(std::unique_ptr<Address> addr);
  void set_addresses(std::vector<std::unique_ptr<Address>> addresses);
  void set_connection(std::unique_ptr<Connection> connection);
  void set_routing(std::unique_ptr<Routing> routing);

  std::vector<std::unique_ptr<Interface>>& get_active_connections();
  const std::unique_ptr<Address>& get_address() const;
  const std::vector<std::unique_ptr<Address>>& get_addresses() const;
  const Connection& get_connection() const;
  Connection& get_connection();
  Routing& get_routing();

 private:
  std::vector<std::unique_ptr<Address>> addresses_;

  // Active interfaces on this node. Pointer due to cyclic nature of Interfaces.
  std::vector<std::unique_ptr<Interface>> active_connections_;
  std::unique_ptr<Connection> connection_;
  std::unique_ptr<Routing> routing_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
