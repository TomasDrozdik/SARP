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
  UpdateInterfaces();
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

void Network::UpdateInterfaces() {
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
    const PositionCube node_position_cube(node->get_position());
    for (uint32_t i = 0; i < neighbor_count; ++i) {
      PositionCube neighbor_cube = node_position_cube + relative_neighbors[i];
      CubeID neighbor_cube_id = neighbor_cube.GetID();
      for (auto &neighbor : node_placement_[neighbor_cube_id]) {
        CheckInterface(*node, *neighbor);
      }
    }
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

void Network::CheckInterface(Node &node1, Node &node2) {
  // Create template Interface for finding existing ones.
  auto key = std::make_unique<Interface>(node1, node2);
  auto search = node1.get_active_interfaces().find(key);
  if (search == node1.get_active_interfaces().end() || !(*search)->is_valid_) {
    // If it doesn't exit or there is an existing one with already
    // destroyed other side then create a new one.
    Interface::Create(node1, node2);
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
  // Print all the edges. Go through all interfaces.
  for (auto &node : nodes_) {
    for (auto &iface : node->get_active_interfaces()) {
      if (node.get() != &iface->get_other_end_node()) {
        os << '\t' << node->get_address() << " -- "
           << iface->get_other_end_node().get_address() << '\n';
      }
    }
  }
  os << "}\n";
}

}  // namespace simulation
