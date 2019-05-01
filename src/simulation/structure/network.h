//
// network.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NETWORK_H_
#define SARP_SIMULATION_STRUCTURE_NETWORK_H_

#include <memory>
#include <vector>

#include "node.h"

namespace simulation {

using Time = size_t;

class Network {
 friend class Simulation;
 public:
  Network(std::vector<std::unique_ptr<Node>> nodes);
  ~Network() = default;

  const std::vector<std::unique_ptr<Node>>& get_nodes() const;
  std::vector<std::unique_ptr<Node>>& get_nodes();
 private:
  std::vector<std::unique_ptr<Node>> nodes_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NETWORK_H_
