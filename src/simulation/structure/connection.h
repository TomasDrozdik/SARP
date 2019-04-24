//
// connection.h
//

#ifndef SARP_SIMULATION_STRUCTURE_CONNECTION_H_
#define SARP_SIMULATION_STRUCTURE_CONNECTION_H_

#include <memory>
#include <vector>

#include "node.h"

namespace simulation {

class Node;

class Connection {
 public:
  Connection(const Node &node) : node_(node) {}
  virtual ~Connection() = 0;

  virtual std::unique_ptr<std::vector<Node*>> GetConnectedNodes(
      const std::vector<std::unique_ptr<Node>> &all_nodes) = 0;

 private:
  const Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
