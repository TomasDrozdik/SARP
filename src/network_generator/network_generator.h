//
// network_generator.cc
//

#ifndef SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_

#include <vector>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "address_iterator.h"
#include "position_generator.h"
#include "../structure/network.h"
#include "../structure/node.h"

namespace simulation {

template <class AddressType, class RoutingType, class ConnectionType>
class NetworkGenerator {
 public:
  NetworkGenerator() {
		// Initialize the pseudo-random generator with time as a seed.
		std::srand(std::time(0));
	}

  std::unique_ptr<Network> Create(uint node_count,
			PositionGenerator &pos_generator) {
    AddressIterator<AddressType> address_iterator;
		std::vector<std::unique_ptr<Node>> nodes;
		// Create nodes
    for (std::size_t i = 0; i < node_count; ++i) {
      nodes.push_back(CreateNode(++address_iterator, ++pos_generator));
		}
		// Create a network
		return std::make_unique<Network>(std::move(nodes));
	}

 private:
	std::unique_ptr<Node> CreateNode(std::unique_ptr<AddressType> addr,
			Position position) {
		auto node = std::make_unique<Node>();
		node->add_address(std::move(addr));

		auto connection = std::make_unique<ConnectionType>(*node, position);
		node->set_connection(std::move(connection));

		auto routing = std::make_unique<RoutingType>(*node);
		node->set_routing(std::move(routing));

		assert(node->IsInitialized());
		return std::move(node);
	}
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_NETWORK_GENERATOR_H_
