//
// network.cc
//

#include "structure/network.h"

#include <algorithm>
#include <iostream>

namespace simulation {

Node &Network::AddNode(const Parameters &parameters, std::unique_ptr<Node> node) {
  nodes_.push_back(std::move(node));
  PlaceNode(parameters, *nodes_.back());
  return *nodes_.back();
}

void Network::UpdateNodePosition(const Parameters &parameters, const Node &node,
    const Position &old_position) {
  // Check if update it is necessary.
  const auto connection_range = parameters.get_connection_range();
  PositionCube old_cube(old_position, connection_range);
  PositionCube new_cube(node.get_position(), connection_range);
  if (old_cube == new_cube) {
    return;
  }
  // Remove it from last position.
  auto pos_boundaries = parameters.get_position_boundaries();
  CubeID old_cube_id = old_cube.GetID(pos_boundaries.first,
      pos_boundaries.second, connection_range);
  std::size_t items_removed = node_placement_[old_cube_id].erase(
      const_cast<Node *>(&node));  // Effectively const.
  assert(items_removed == 1);
  // Set new position cube and add it to new place.
  CubeID new_cube_id = new_cube.GetID(pos_boundaries.first,
      pos_boundaries.second, connection_range);
  // Node ptr is effectively const.
  auto [it, success] = node_placement_[new_cube_id]
      .insert(const_cast<Node *>(&node));
  assert(success);
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
    const PositionCube node_cube(node->get_position(),
                                 env.parameters.get_connection_range());
    for (uint32_t i = 0; i < neighbor_count; ++i) {
      auto [neighbor_cube, success] =
          node_cube.GetRelativeCube(relative_neighbors[i]);
      if (!success) {
        continue;
      }
      CubeID neighbor_cube_id =
          neighbor_cube.GetID(env.parameters.get_position_boundaries().first,
                              env.parameters.get_position_boundaries().second,
                              env.parameters.get_connection_range());
      auto node_set_it = node_placement_.find(neighbor_cube_id);
      if (node_set_it != node_placement_.end()) {
        for (Node *neighbor : node_set_it->second) {
          if (node->IsConnectedTo(*neighbor,
                                 env.parameters.get_connection_range())) {
            new_neighbors.insert(neighbor);
          }
        }
      }
    }
    node->UpdateNeighbors(env, new_neighbors);
  }
}

void Network::PlaceNode(const Parameters &parameters, Node &node) {
  auto connection_range = parameters.get_connection_range();
  PositionCube cube{node.get_position(), connection_range};
  auto min_pos = parameters.get_position_boundaries().first;
  auto max_pos = parameters.get_position_boundaries().second;
  auto cube_id = cube.GetID(min_pos, max_pos, connection_range);
  auto &cube_set = node_placement_[cube_id];
  const auto [it, success] = cube_set.insert(&node);
  assert(success);
}

void Network::ExportToDot(std::ostream &os) const {
  // Mark this as strict graph to remove duplikecate edges.
  os << "strict graph G {\n";
  // Assign position to all nodes.
  for (auto &node : nodes_) {
    os << '\t' << node->get_address() << " [ " << node->get_position() << " ]\n";
  }
  // Print all the edges. Go through all neighbors.
  for (auto &node : nodes_) {
    for (Node *neighbor : node->get_neighbors()) {
      if (node.get() != neighbor) {
        os << '\t' << node->get_address() << " -- " << neighbor->get_address()
           << '\n';
      }
    }
  }
  os << "}\n";
}

}  // namespace simulation
