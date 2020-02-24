# SARP (Scalable-addressing-and-routing-protocol-for-ad-hoc-networks) - Programming part
The aim of the thesis is to design a highly scalable decentralized network addressing and routing protocol for ad-hoc networks. The network addresses will be assigned and dynamically reassigned so that the nodes are kept in a loose tree-like hierarchy of addressable groups. The design will differ from other such protocols by being completely stateless (to aid reliability) and able to organize the network in a way that minimizes the sizes of the routing tables on all nodes (to improve efficiency). The resulting protocol will be implemented in a matching network simulator for being tested, benchmarked, and possibly compared with other protocols.

# Build
```
cd build
cmake ..
cmake --build .
```

# Run
There are several binaries in bin folder.
```
> ls bin
distance_vector_simulation
sarp_simulation
static_routing_simulation
network.dot
network_initial.dot
```

`*.dot` Files are for either manual configuration or just a note on what the
simulation has been running
