//
// network.cc
//

#include "structure/network.h"

#include <algorithm>
#include <iostream>

namespace simulation {

Network::Network(std::vector<Node> &&nodes) : nodes_(std::move(nodes)) {}

void Network::Init(Env &env) {
  // Place all nodes to appropriate position cubes.
  InitializeNodePlacement(env.parameters.position_min,
                          env.parameters.position_max,
                          env.parameters.connection_range);
  // Initialize the network connections.
  UpdateNeighbors(env);
  // Initialize network routing.
  for (auto &node : nodes_) {
    node.get_routing().Init(env);
  }
}

void Network::UpdateNodePosition(const Node &node,
                                 PositionCube new_position_cube,
                                 Position min_pos, Position max_pos,
                                 uint32_t connection_range) {
  // Remove it from last position.
  PositionCube old_cube(node.get_position(), connection_range);
  CubeID old_cube_id = old_cube.GetID(min_pos, max_pos, connection_range);
  std::size_t items_removed = node_placement_[old_cube_id].erase(
      const_cast<Node *>(&node));  // Effectively const.
  assert(items_removed == 1);
  // Set new position cube and add it to new place.
  auto pair = node_placement_[new_position_cube.GetID(min_pos, max_pos,
                                                      connection_range)]
                  .insert(const_cast<Node *>(&node));
  assert(pair.second);  // Insertion was successful.
}

// Friend method of Node -> can update neighbors
void Network::UpdateNeighbors(Env &env) {
  const uint32_t neighbor_count = 27;
  const int relative_neighbors[neighbor_count][3] = {
      {-1, -1, -1}, {-1, -1, 0}, {-1, -1, 1}, {-1, 0, -1}, {-1, 0, 0},
      {-1, 0, 1},   {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1},  {0, -1, -1},
      {0, -1, 0},   {0, -1, 1},  {0, 0, -1},  {0, 0, 0},   {0, 0, 1},
      {0, 1, -1},   {0, 1, 0},   {0, 1, 1},   {1, -1, -1}, {1, -1, 0},
      {1, -1, 1},   {1, 0, -1},  {1, 0, 0},   {1, 0, 1},   {1, 1, -1},
      {1, 1, 0},    {1, 1, 1}};
  for (auto &node : nodes_) {
    std::set<Node *> new_neighbors;
    const PositionCube node_cube(node.get_position(),
                                 env.parameters.connection_range);
    for (uint32_t i = 0; i < neighbor_count; ++i) {
      auto [neighbor_cube, success] =
          node_cube.GetRelativeCube(relative_neighbors[i]);
      if (!success) {
        continue;
      }
      CubeID neighbor_cube_id = neighbor_cube.GetID(
          env.parameters.position_min, env.parameters.position_max,
          env.parameters.connection_range);
      for (Node *neighbor : node_placement_[neighbor_cube_id]) {
        if (node.IsConnectedTo(*neighbor, env.parameters.connection_range)) {
          new_neighbors.insert(neighbor);
        }
      }
    }
    node.UpdateNeighbors(env, new_neighbors);
  }
}

void Network::InitializeNodePlacement(Position min_pos, Position max_pos,
                                      uint32_t connection_range) {
  for (auto &node : nodes_) {
    auto pair =
        node_placement_[PositionCube(node.get_position(), connection_range)
                            .GetID(min_pos, max_pos, connection_range)]
            .insert(&node);
    assert(pair.second);  // Insertion was sucessful.
  }
}

void Network::ExportToDot(std::ostream &os) const {
  // Mark this as strict graph to remove duplicate edges.
  os << "strict graph G {\n";
  // Assign position to all nodes.
  for (auto &node : nodes_) {
    os << '\t' << node.get_address() << " [ " << node.get_position() << " ]\n";
  }
  // Print all the edges. Go through all neighbors.
  for (auto &node : nodes_) {
    for (Node *neighbor : node.get_neighbors()) {
      if (&node != neighbor) {
        os << '\t' << node.get_address() << " -- " << neighbor->get_address()
           << '\n';
      }
    }
  }
  os << "}\n";
}

}  // namespace simulation
