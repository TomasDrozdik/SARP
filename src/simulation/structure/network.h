//
// network.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NETWORK_H_
#define SARP_SIMULATION_STRUCTURE_NETWORK_H_

#include <memory>
#include <vector>

#include "simulation.h"
#include "statistics.h"
#include "node.h"

namespace simulation {

using Time = size_t;
class Event;

class Network {
 friend class Simulation;
 public:
  Network(std::unique_ptr<std::vector<std::unique_ptr<Node>>> nodes,
      std::unique_ptr<std::vector<std::unique_ptr<Event>>> events);
  ~Network() = default;

 private:
  std::unique_ptr<std::vector<std::unique_ptr<Node>>> nodes_;
  std::unique_ptr<std::vector<std::unique_ptr<Event>>> events_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NETWORK_H_
