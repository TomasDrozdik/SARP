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
class NetworkGenerator {
 public:
  NetworkGenerator() {
    // Initialize the pseudo-random generator with time as a seed.
    std::srand(std::time(0));
  }

  std::unique_ptr<Network> Create(
      uint node_count, std::unique_ptr<PositionGenerator> pos_generator,
      AddressGenerator &&address_generator) {
    std::vector<std::unique_ptr<Node>> nodes;
    // Create nodes
    for (std::size_t i = 0; i < node_count; ++i) {
      Position pos = ++(*pos_generator);
      nodes.push_back(CreateNode(address_generator.GenerateAddress(pos), pos));
    }
    // Create a network
    return std::make_unique<Network>(std::move(nodes));
  }

 private:
  std::unique_ptr<Node> CreateNode(Address addr, Position position) {
    auto node = std::make_unique<Node>();
    node->add_address(addr);
    node->set_position(position);

    auto routing = std::make_unique<RoutingType>(*node);
    node->set_routing(std::move(routing));

    assert(node->IsInitialized());
    return node;
  }
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_
