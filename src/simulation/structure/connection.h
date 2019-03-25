//
// connection.h
//

#ifndef SARP_SIMULATION_STRUCTURE_CONNECTION_H_
#define SARP_SIMULATION_STRUCTURE_CONNECTION_H_

#include <memory>
#include <set>

#include "node.h"

namespace simulation {

class Connection {
 public:
  Connection(const Node &node) : node_(node) {}
  virtual ~Connection() = 0;

  virtual std::unique_ptr<std::set<Node const * const>>&& GetConnectedNodes(
      const std::set<Node const * const> &all_nodes) = 0;

 private:
  const Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
