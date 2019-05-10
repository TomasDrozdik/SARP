//
// connection.h
//

#ifndef SARP_SIMULATION_STRUCTURE_CONNECTION_H_
#define SARP_SIMULATION_STRUCTURE_CONNECTION_H_

#include <memory>
#include <vector>

#include "node.h"
#include "position.h"

namespace simulation {

class Node;

class Connection {
 public:
  virtual std::vector<Node*> GetConnectedNodes(
      const std::vector<std::unique_ptr<Node>> &all_nodes) = 0;

  Position position;
 protected:
  Connection(const Node &node, const Position position);
  virtual ~Connection() = 0;

  const Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
