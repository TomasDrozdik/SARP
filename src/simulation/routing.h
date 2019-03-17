//
// routing.h
//

#ifndef SARP_SMULATION_ROUTING_H_
#define SARP_SMULATION_ROUTING_H_

#include "address.h"
#include "interface.h"

namespace simulation {

class Routing {
 public:
  virtual Interface& Route(const Address &addr) const = 0;

 protected:
  Routing(const Node &node);

 private:
  const Node &node_;
};

}  // namespace simulation

#endif  // SARP_SMULATION_ROUTING_H_
