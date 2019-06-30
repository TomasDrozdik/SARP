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

struct UniquePtrInteraceEqualTo {
  bool operator()(const std::unique_ptr<simulation::Interface> &i1,
      const std::unique_ptr<simulation::Interface> &i2) const;
};

class Node {
 friend std::ostream &operator<<(std::ostream &os, const Node &node);
 public:
  using InterfaceContainerType = std::unordered_set<std::unique_ptr<Interface>,
      UniquePtrInterfaceHash, UniquePtrInteraceEqualTo>;
  using AddressContainerType = std::vector<std::unique_ptr<Address>>;

  Node();
  ~Node() = default;

  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet,
      Interface *processing_interface);
  bool IsInitialized() const;

  void add_address(std::unique_ptr<Address> addr);
  void set_addresses(AddressContainerType addresses);
  void set_connection(std::unique_ptr<Connection> connection);
  void set_routing(std::unique_ptr<Routing> routing);

  InterfaceContainerType &get_active_interfaces();
  const std::unique_ptr<Address> &get_address() const;
  const   AddressContainerType &get_addresses() const;
  const Connection& get_connection() const;
  Connection &get_connection();
  Routing &get_routing();

 private:
  AddressContainerType addresses_;
  InterfaceContainerType active_interfaces_;
  std::unique_ptr<Connection> connection_;
  std::unique_ptr<Routing> routing_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
