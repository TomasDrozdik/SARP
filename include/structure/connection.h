//
// connection.h
//

#ifndef SARP_STRUCTURE_CONNECTION_H_
#define SARP_STRUCTURE_CONNECTION_H_

#include <memory>
#include <vector>

#include "structure/node.h"
#include "structure/position.h"

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

#endif  // SARP_STRUCTURE_CONNECTION_H_
