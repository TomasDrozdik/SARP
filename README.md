# SARP (Scalable-addressing-and-routing-protocol-for-ad-hoc-networks) - Programming part
The aim of the thesis is to design a highly scalable decentralized network addressing and routing protocol for ad-hoc networks. The network addresses will be assigned and dynamically reassigned so that the nodes are kept in a loose tree-like hierarchy of addressable groups. The design will differ from other such protocols by being completely stateless (to aid reliability) and able to organize the network in a way that minimizes the sizes of the routing tables on all nodes (to improve efficiency). The resulting protocol will be implemented in a matching network simulator for being tested, benchmarked, and possibly compared with other protocols.

# Build

Build all binaries
`make`

Remake
`make remake`

Debug config with asserts
`make debug`

Linter using `clang-format -style=Google`
```
make cstyle  # prints diff
make fix-cstyle
```

# Run

There are several binaries in bin folder.
```
> ls bin
distance_vector
sarp
sarp_linear
sarp_square
sarp_cube
sarp_bigcube
sarp_readdress_square
sarp_readdress_cube
sarp_update_threshold
```

Run all binaries - may take long time apprx. 2 hours
`make data`

Plot all thesis plots
`make plot`

## Configuration

### Simulation parameters

Most of the input parameters of the simulation are static in class `simulation::SimulationParameters` these can be set only programaticaly.

### Network Topology

Topology is stored in `*.dot` files.
So far this topology may be stored at the beginning and at the end of the
simulation.

To convert it to `.png` use the following:
`neato -Tpng -o <output.png> <network.dot>

