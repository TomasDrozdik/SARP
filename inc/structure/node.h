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
#include <set>

#include "structure/types.h"
#include "structure/packet.h"
#include "structure/position.h"
#include "structure/routing.h"

namespace simulation {

struct Env;
class Routing;
class Packet;
class Parameters;

class Node final {
  friend std::ostream &operator<<(std::ostream &os, const Node &node);

 public:
  struct MobilityPlan {
    Position destination;
    double speed;
    Time pause;
  };

  using AddressContainerType = std::set<Address>;
  using ID = std::size_t;

  Node() : id_(Node::next_id_++) {}

  Node(Node &&node) { *this = std::move(node); }  // use operator==(Node &&)

  Node &operator=(Node &&);

  // WARNING: No copy constructor and copy assignment operator  due to nature of
  // unique id_ and routing.

  bool operator==(const Node &other) const { return id_ == other.id_; }

  void Send(Env &env, std::unique_ptr<Packet> packet);

  void Recv(Env &env, std::unique_ptr<Packet> packet, Node *form_node);

  bool IsInitialized() const { return routing_ != nullptr; }

  bool IsConnectedTo(const Node &node, uint32_t connection_range) const;

  void UpdateNeighbors(Env &env, std::set<Node *> new_neighbors);

  ID get_id() const { return id_; }


  Position get_position() const { return position_; }

  void AddAddress(Address addr);

  void InitializeAddresses(Node::AddressContainerType addresses);

  void Move(Time time_diff);

  bool has_mobility_plan() const { return mobility_.first; }

  void set_mobility_plan(const MobilityPlan &new_plan) {
    mobility_ = {true, new_plan};
  }

  void set_position(Position position) { position_ = position; }

  const Address get_address() const {
    return (addresses_.size() == 0) ? Address() : *latest_address_;
  }

  const AddressContainerType &get_addresses() const {
    assert(IsInitialized());
    return addresses_;
  }

  const std::set<Node *> &get_neighbors() const { return neighbors_; }

  void set_routing(std::unique_ptr<Routing> routing) {
    routing_ = std::move(routing); }

  const Routing &get_routing() const {
    assert(IsInitialized());
    return *routing_;
  }

  Routing &get_routing() {
    assert(IsInitialized());
    return *routing_;
  }

 private:
  static inline ID next_id_ = 0;

  ID id_;
  Position position_;
  AddressContainerType::iterator latest_address_;
  AddressContainerType addresses_;
  std::set<Node *> neighbors_;
  std::unique_ptr<Routing> routing_ = nullptr;
  std::pair<bool, MobilityPlan> mobility_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NODE_H_
