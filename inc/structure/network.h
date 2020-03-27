//
// network.h
//

#ifndef SARP_STRUCTURE_NETWORK_H_
#define SARP_STRUCTURE_NETWORK_H_

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "structure/node.h"
#include "structure/position_cube.h"

namespace simulation {

class Node;
class PositionCube;

class Network {
  friend class Simulation;

 public:
  // Initialize network with given set of nodes.
  // Also initialize interfaces on each node and based on that initialize
  // routing.
  Network(std::vector<std::unique_ptr<Node>> nodes);
  ~Network() = default;

  // Updates active_interfaces_ on all nodes.
  // Do not delete any interfaces since they migh be still in use by Routing.
  // therefore leave deletion of invalid interfaces to Routing update.
  // Add only new ones.
  void UpdateInterfaces();

  // Exports the network to .dot format to given output stream.
  void ExportToDot(std::ostream &os) const;

  // Assumes node has old position still set.
  void UpdateNodePosition(const Node &node, PositionCube new_position_cube);

  const std::vector<std::unique_ptr<Node>> &get_nodes() const { return nodes_; }

  std::vector<std::unique_ptr<Node>> &get_nodes() { return nodes_; }

 private:
  using CubeID = std::size_t;

  void InitializeNodePlacement();

  void CheckInterface(Node &node, Node &other);

  std::vector<std::unique_ptr<Node>> nodes_;
  std::map<CubeID, std::set<Node *>> node_placement_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NETWORK_H_
