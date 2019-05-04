//
// static_routing.h
//

#ifndef SARP_SIMULATION_STATIC_FORWARDING_STATIC_ROUTING_H_
#define SARP_SIMULATION_STATIC_FORWARDING_STATIC_ROUTING_H_

#include <vector>
#include <map>

#include "../structure/interface.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class StaticRouting final : public Routing {
 public:
  StaticRouting(Node& node);

  Interface * const Route(const Address &addr) const override;

  bool ConnectToNode(const Node &node);

 private:
  std::map<SimpleAddress, Interface*> mapping_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STATIC_FORWARDING_STATIC_ROUTING_H_
