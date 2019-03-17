//
// network.h
//

#ifndef SARP_SIMUALTION_NETWOR_H_
#define SARP_SIMUALTION_NETWOR_H_

#include <vector>

#include "node.h"

namespace simulation {

class NetworkFactory {
 public:

 private:

};

class Network {
 // TODO
 friend class NetwordFactory;

 public:
  std::vector<Node> nodes;
};

}  // namespace simulation

#endif  // SARP_SIMUALTION_NETWOR_H_
