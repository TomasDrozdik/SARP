//
// network.cc
//

#include "structure/network.h"

#include <algorithm>
#include <iostream>

#include "structure/simulation.h"

namespace simulation {

Network::Network(std::vector<std::unique_ptr<Node>> nodes)
    : nodes_(std::move(nodes)) {
  // Check if parameters mainy connection_range are initialized.
  if (!SimulationParameters::IsMandatoryInitialized()) {
    std::cerr << "Simulation Parameters uninitialized\n";
  }
  // Place all nodes to appropriate position cubes.
  InitializeNodePlacement();
  // Initialize the network connections.
  UpdateNeighbors();
  // Initialize network routing.
  for (auto &node : nodes_) {
    node->get_routing().Init();
  }
}

void Network::UpdateNodePosition(const Node &node,
                                 PositionCube new_position_cube) {
  // Remove it from last position.
  PositionCube old_cube(node.get_position());
  CubeID old_cube_id = old_cube.GetID();
  std::size_t items_removed = node_placement_[old_cube_id].erase(
      const_cast<Node *>(&node));  // Effectively const.
  assert(items_removed == 1);
  // Set new position cube and add it to new place.
  auto pair = node_placement_[new_position_cube.GetID()].insert(
      const_cast<Node *>(&node));
  assert(pair.second);  // Insertion was successful.
}

// Friend method of Node -> can update neighbors
void Network::UpdateNeighbors() {
  const uint32_t neighbor_count = 27;
  const PositionCube relative_neighbors[neighbor_count] = {
      PositionCube(-1, -1, -1), PositionCube(-1, -1, 0),
      PositionCube(-1, -1, 1),  PositionCube(-1, 0, -1),
      PositionCube(-1, 0, 0),   PositionCube(-1, 0, 1),
      PositionCube(-1, 1, -1),  PositionCube(-1, 1, 0),
      PositionCube(-1, 1, 1),   PositionCube(0, -1, -1),
      PositionCube(0, -1, 0),   PositionCube(0, -1, 1),
      PositionCube(0, 0, -1),   PositionCube(0, 0, 0),
      PositionCube(0, 0, 1),    PositionCube(0, 1, -1),
      PositionCube(0, 1, 0),    PositionCube(0, 1, 1),
      PositionCube(1, -1, -1),  PositionCube(1, -1, 0),
      PositionCube(1, -1, 1),   PositionCube(1, 0, -1),
      PositionCube(1, 0, 0),    PositionCube(1, 0, 1),
      PositionCube(1, 1, -1),   PositionCube(1, 1, 0),
      PositionCube(1, 1, 1)};
  for (auto &node : nodes_) {
    std::set<Node *> new_neighbors;
    const PositionCube node_position_cube(node->get_position());
    for (uint32_t i = 0; i < neighbor_count; ++i) {
      PositionCube neighbor_cube = node_position_cube + relative_neighbors[i];
      CubeID neighbor_cube_id = neighbor_cube.GetID();
      for (auto &neighbor : node_placement_[neighbor_cube_id]) {
        new_neighbors.insert(neighbor);
      }
    }
    node->UpdateNeighbors(new_neighbors);
  }
}

void Network::InitializeNodePlacement() {
  for (auto &node : nodes_) {
    auto pair =
        node_placement_[PositionCube(node->get_position()).GetID()].insert(
            node.get());
    assert(pair.second);  // Insertion was sucessful.
  }
}

void Network::ExportToDot(std::ostream &os) const {
  // Mark this as strict graph to remove duplicate edges.
  os << "strict graph G {\n";
  // Assign position to all nodes.
  for (auto &node : nodes_) {
    os << '\t' << node->get_address() << " [ " << node->get_position()
       << " ]\n";
  }
  // Print all the edges. Go through all neighbors.
  for (auto &node : nodes_) {
    for (auto neighbor_ptr : node->get_neighbors()) {
      if (node.get() != neighbor_ptr) {
        os << '\t' << node->get_address() << " -- "
           << neighbor_ptr->get_address() << '\n';
      }
    }
  }
  os << "}\n";
}

}  // namespace simulation
