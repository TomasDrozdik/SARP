//
// node.h
//

#ifndef SARP_STRUCTURE_NODE_H_
#define SARP_STRUCTURE_NODE_H_

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "structure/address.h"
#include "structure/interface.h"
#include "structure/network.h"
#include "structure/position.h"
#include "structure/routing.h"
#include "structure/simulation.h"

namespace simulation {

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
  using InterfaceContainerType =
      std::unordered_set<std::unique_ptr<Interface>, UniquePtrInterfaceHash,
                         UniquePtrInterfaceEqualTo>;
  using AddressContainerType = std::vector<Address>;

  Node();
  Node(Node &&node);

  Node &operator=(Node &&);

  // WARNING: No copy constructor and copy assignment operator  due to nature of
  // unique id_.

  void Send(std::unique_ptr<ProtocolPacket> packet);
  void Recv(std::unique_ptr<ProtocolPacket> packet,
            Interface *processing_interface);

  bool operator==(const Node &other) const { return id_ == other.id_; }

  bool IsInitialized() const { return !addresses_.empty() && routing_; }

  bool IsConnectedTo(const Node &node) const;

  void set_position(Position pos) { position_ = pos; }

  const Position &get_position() const { return position_; }

  void add_address(Address addr) { addresses_.push_back(addr); }

  void set_addresses(Node::AddressContainerType addresses) {
    addresses_ = addresses;
  }

  void set_routing(std::unique_ptr<Routing> routing) {
    routing_ = std::move(routing);
  }

  InterfaceContainerType &get_active_interfaces() { return active_interfaces_; }

  const InterfaceContainerType &get_active_interfaces() const {
    return active_interfaces_;
  }

  const Address &get_address() const {
    assert(IsInitialized());
    return addresses_[0];
  }

  const AddressContainerType &get_addresses() const {
    assert(IsInitialized());
    return addresses_;
  }

  Routing &get_routing() {
    assert(IsInitialized());
    return *routing_;
  }

 private:
  static inline size_t id_counter_ = 0;

  size_t id_;
  Position position_;
  AddressContainerType addresses_;
  InterfaceContainerType active_interfaces_;
  std::unique_ptr<Routing> routing_ = nullptr;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
