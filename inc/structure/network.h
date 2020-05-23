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
#include "structure/position.h"
#include "structure/simulation.h"

namespace simulation {

class Node;
struct Env;
class PositionCube;
class Parameters;

class Network final {
  friend class Simulation;
  using NodeContainer = std::vector<std::unique_ptr<Node>>;

 public:
  Node &AddNode(const Parameters &parameters, std::unique_ptr<Node> node);

  void UpdateNeighbors(Env &env);

  // Exports the network to .dot format to given output stream.
  void ExportToDot(std::ostream &os) const;

  // Assumes node has old position still set.
  void UpdateNodePosition(const Node &node, PositionCube new_position_cube,
                          Position max_pos, Position min_pos,
                          uint32_t connection_range);

  const NodeContainer &get_nodes() const { return nodes_; }

  NodeContainer &get_nodes() { return nodes_; }

 private:
  using CubeID = std::size_t;

  void PlaceNode(const Parameters &parameters, Node &node);

  NodeContainer nodes_;
  std::map<CubeID, std::set<Node *>> node_placement_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NETWORK_H_
