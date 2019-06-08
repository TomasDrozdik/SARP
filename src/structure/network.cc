//
// network.cc
//
#include <iostream>

#include "network.h"

namespace simulation {

Network::Network(std::vector<std::unique_ptr<Node>> nodes) :
    nodes_(std::move(nodes)) {
  UpdateConnections();
}

void Network::UpdateConnections() {
  // First clear all connections
  for (auto &node : nodes_) {
    node->get_active_connections().clear();
  }
  // Now go through every distinct pair of nodes and if they both see each other
  // via Connection::IsConnectedTo then Create Interface between them.
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    for (std::size_t j = i; j < nodes_.size(); ++j) {
      if (nodes_[i]->get_connection().IsConnectedTo(*nodes_[j]) &&
          nodes_[j]->get_connection().IsConnectedTo(*nodes_[i])) {
        Interface::Create(*nodes_[i], *nodes_[j]);
      }
    }
  }
}

const std::vector<std::unique_ptr<Node>>& Network::get_nodes() const {
  return nodes_;
}

std::vector<std::unique_ptr<Node>>& Network::get_nodes() {
  return nodes_;
}

}  // namespace simulation