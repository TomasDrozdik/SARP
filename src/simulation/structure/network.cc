//
// network.cc
//

#include "network.h"

namespace simulation {

Network::Network(std::vector<std::unique_ptr<Node>> nodes) :
        nodes_(std::move(nodes)) {
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    nodes_[i]->UpdateConnections(nodes_);
  }
}

const std::vector<std::unique_ptr<Node>>& Network::get_nodes() const {
  return nodes_;
}

std::vector<std::unique_ptr<Node>>& Network::get_nodes() {
  return nodes_;
}

}  // namespace simulation