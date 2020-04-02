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
  // Also initialize neighbors on each node.
  Network(std::vector<Node> &&nodes);

  ~Network() = default;

  // Update neighbors.
  // Do Routing::Init on all nodes possibly starting periodic routing update.
  // Should be called by InitNetworkEvent inside of simulation.
  void Init();

  void UpdateNeighbors();

  // Exports the network to .dot format to given output stream.
  void ExportToDot(std::ostream &os) const;

  // Assumes node has old position still set.
  void UpdateNodePosition(const Node &node, PositionCube new_position_cube);

  const std::vector<Node> &get_nodes() const { return nodes_; }

  std::vector<Node> &get_nodes() { return nodes_; }

 private:
  using CubeID = std::size_t;

  void InitializeNodePlacement();

  std::vector<Node> nodes_;
  std::map<CubeID, std::set<Node *>> node_placement_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NETWORK_H_
