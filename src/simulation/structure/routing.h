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
  // Returns nullptr if Routing detects a cycle.
  virtual Interface * const Route(const Address &addr) const = 0;

 protected:
  Routing(Node &node) : node_(node) { }
  Node &node_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ROUTING_H_
