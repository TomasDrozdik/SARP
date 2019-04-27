//
// network.cc
//

#include "network.h"

namespace simulation {

Network::Network(std::unique_ptr<std::vector<std::unique_ptr<Node>>> nodes,
    std::unique_ptr<std::vector<std::unique_ptr<Event>>> events) :
        nodes_(std::move(nodes)), events_(std::move(events)) {
  for (std::size_t i = 0; i < nodes_->size(); ++i) {
    nodes_->operator[](i)->UpdateConnections(*nodes_);
  }
}

}  // namespace simulation