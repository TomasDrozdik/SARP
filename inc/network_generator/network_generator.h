//
// network_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "network_generator/address_generator.h"
#include "network_generator/position_generator.h"
#include "structure/network.h"
#include "structure/node.h"

namespace simulation {

template <class RoutingType>
class NetworkGenerator final {
 public:
  NetworkGenerator() {
    // Initialize the pseudo-random generator with time as a seed.
    std::srand(std::time(0));
  }

  std::unique_ptr<Network> Create(
      uint node_count, std::unique_ptr<PositionGenerator> pos_generator,
      AddressGenerator &&address_generator) {
    std::vector<Node> nodes(node_count);
    // Create nodes
    for (std::size_t i = 0; i < node_count; ++i) {
      Position pos = ++(*pos_generator);
      nodes[i].add_address(address_generator.GenerateAddress(pos));
      nodes[i].set_position(pos);
      nodes[i].set_routing(std::make_unique<RoutingType>(nodes[i]));
      assert(nodes[i].IsInitialized());
    }
    // Create a network
    return std::make_unique<Network>(std::move(nodes));
  }
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_
