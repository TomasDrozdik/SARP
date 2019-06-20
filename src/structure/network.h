//
// network.h
//

#ifndef SARP_STRUCTURE_NETWORK_H_
#define SARP_STRUCTURE_NETWORK_H_

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

  // Updates active_interfaces_ on all nodes.
  // Do not delete any interfaces since they migh be still in use by Routing.
  // therefore leave deletion of invalid interfaces to Routing update.
  // Only add new ones.
  void UpdateInterfaces();

  // Exports the network to .dot format to given output stream.
  void ExportToDot(std::ostream &os) const;

  const std::vector<std::unique_ptr<Node>>& get_nodes() const;
  std::vector<std::unique_ptr<Node>>& get_nodes();
 private:
  std::vector<std::unique_ptr<Node>> nodes_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_NETWORK_H_
