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
  virtual bool IsConnectedTo(const Node &node) const = 0;
  virtual ~Connection() = 0;

  Position position;
 protected:
  Connection(const Node &node, const Position position);

  const Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_CONNECTION_H_
