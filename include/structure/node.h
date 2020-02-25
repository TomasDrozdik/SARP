//
// node.h
//

#ifndef SARP_STRUCTURE_NODE_H_
#define SARP_STRUCTURE_NODE_H_

#include <functional>
#include <memory>
#include <vector>
#include <unordered_set>

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

struct UniquePtrInterfaceHash {
  std::size_t operator()(const std::unique_ptr<simulation::Interface> &i) const;
};

struct UniquePtrInterfaceEqualTo {
  bool operator()(const std::unique_ptr<simulation::Interface> &i1,
      const std::unique_ptr<simulation::Interface> &i2) const;
};

class Node {
 friend std::ostream &operator<<(std::ostream &os, const Node &node);
 public:
  using InterfaceContainerType = std::unordered_set<std::unique_ptr<Interface>,
      UniquePtrInterfaceHash, UniquePtrInterfaceEqualTo>;
  using AddressContainerType = std::vector<std::unique_ptr<Address>>;

  Node();
  Node(Node &&node);

  Node &operator=(Node &&);

  // WARNING: No copy constructor and copy assignment operator  due to nature of
  // unique id_.

  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet,
      Interface *processing_interface);

  bool operator==(const Node &other) const {
    return id_ == other.id_;
  }

  bool IsInitialized() const {
    return !addresses_.empty() && routing_ && connection_;
  }

  void add_address(std::unique_ptr<Address> addr) {
    addresses_.push_back(std::move(addr));
  }

  void set_addresses(Node::AddressContainerType addresses) {
    addresses_ = std::move(addresses);
  }

  void set_connection(std::unique_ptr<Connection> connection) {
    connection_ = std::move(connection);
  }

  void set_routing(std::unique_ptr<Routing> routing) {
    routing_ = std::move(routing);
  }

  InterfaceContainerType &get_active_interfaces() {
    return active_interfaces_;
  }

  const InterfaceContainerType &get_active_interfaces() const {
    return active_interfaces_;
  }

  const std::unique_ptr<Address> &get_address() const {
    return addresses_[0];
  }

  const AddressContainerType &get_addresses() const {
    return addresses_;
  }

  const Connection &get_connection() const {
    return *connection_;
  }

  Connection &get_connection() {
    return *connection_;
  }

  Routing &get_routing() {
    return *routing_;
  }
  
 private:
  static inline size_t id_counter_ = 0;
  size_t id_;
  AddressContainerType addresses_;
  InterfaceContainerType active_interfaces_;
  std::unique_ptr<Connection> connection_ = nullptr;
  std::unique_ptr<Routing> routing_ = nullptr;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
