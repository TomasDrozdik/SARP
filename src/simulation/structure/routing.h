//
// routing.h
//

#ifndef SARP_SIMULATION_STRUCTURE_ROUTING_H_
#define SARP_SIMULATION_STRUCTURE_ROUTING_H_

#include "address.h"
#include "interface.h"

namespace simulation {

class Interface;
class Node;

class Routing {
 public:
  virtual ~Routing() = 0;
  // Initializes routing self configuration.
  // Should be called just after the network is generated when all nodes are
  // already placed at their positions at the same time for all the nodes.
  virtual void Init() = 0;

  // Returns nullptr if Routing detects a cycle.
  virtual Interface * const Route(const Address &addr) const = 0;
 protected:
  Routing(Node &node);

  Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ROUTING_H_
