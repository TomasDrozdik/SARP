//
// network.cc
//
#include <iostream>

#include "network.h"

namespace simulation {

Network::Network(std::vector<std::unique_ptr<Node>> nodes) :
    nodes_(std::move(nodes)) {
  // Initialize the network connections
  UpdateConnections();
  // Initialize network routing
  for (auto &node : nodes_) {
    node->get_routing().Init();
  }
  // Do the first Update() on network
  for (auto &node : nodes_) {
    node->get_routing().Update();
  }
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

void Network::ExportToDot(std::ostream &os) const {
  // Mark this as strict graph to remove duplicate edges.
  os << "strict graph G {\n";
  // Assign position to all nodes.
  for (auto &node : nodes_) {
    os << '\t' << *node->get_address() << " [ " <<
        node->get_connection().position << " ]\n";
  }
  // Print all the edges. Go through all interfaces.
  for (auto &node : nodes_) {
    for (auto &iface : node->get_active_connections()) {
      if (node.get() != &iface->get_other_end_node()) {
        os << '\t' << *node->get_address() << " -- " <<
            *iface->get_other_end_node().get_address() << '\n';
      }
    }
  }
  os << "}\n";
}

const std::vector<std::unique_ptr<Node>>& Network::get_nodes() const {
  return nodes_;
}

std::vector<std::unique_ptr<Node>>& Network::get_nodes() {
  return nodes_;
}

}  // namespace simulation