//
// node.h
//

#ifndef SARP_STRUCTURE_NODE_H_
#define SARP_STRUCTURE_NODE_H_

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "structure/address.h"
#include "structure/network.h"
#include "structure/packet.h"
#include "structure/position.h"
#include "structure/routing.h"
#include "structure/simulation.h"

namespace simulation {

class Routing;
class Packet;
struct Env;

class Node final {
  friend std::ostream &operator<<(std::ostream &os, const Node &node);

 public:
  using AddressContainerType = std::set<Address>;

  Node();
  Node(Node &&node);

  Node &operator=(Node &&);

  // WARNING: No copy constructor and copy assignment operator  due to nature of
  // unique id_.

  void Send(Env &env, std::unique_ptr<Packet> packet);
  void Recv(Env &env, std::unique_ptr<Packet> packet, Node *form_node);

  bool operator==(const Node &other) const { return id_ == other.id_; }

  bool IsInitialized() const { return routing_ != nullptr; }

  bool IsConnectedTo(const Node &node, uint32_t connection_range) const;

  void UpdateNeighbors(Env &env, std::set<Node *> neighbors);

  void set_position(Position position) { position_ = position; }

  Position get_position() const { return position_; }

  void add_address(Address addr) {
    auto pair = addresses_.insert(addr);
    latest_address_ = pair.first;
  }

  void set_addresses(Node::AddressContainerType addresses) {
    addresses_ = addresses;
    latest_address_ = addresses_.begin();
  }

  const Address get_address() const {
    return (addresses_.size() == 0) ? Address() : *latest_address_;
  }

  const AddressContainerType &get_addresses() const {
    assert(IsInitialized());
    return addresses_;
  }

  const std::set<Node *> &get_neighbors() const { return neighbors_; }

  void set_routing(std::unique_ptr<Routing> routing) {
    routing_ = std::move(routing);
  }

  const Routing &get_routing() const {
    assert(IsInitialized());
    return *routing_;
  }

  Routing &get_routing() {
    assert(IsInitialized());
    return *routing_;
  }

 private:
  static inline size_t id_counter_ = 0;

  size_t id_;
  Position position_;
  AddressContainerType::iterator latest_address_;
  AddressContainerType addresses_;
  std::set<Node *> neighbors_;
  std::unique_ptr<Routing> routing_ = nullptr;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
