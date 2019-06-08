//
// static_routing.h
//

#ifndef SARP_STATIC_FORWARDING_STATIC_ROUTING_H_
#define SARP_STATIC_FORWARDING_STATIC_ROUTING_H_

#include <vector>
#include <map>

#include "../structure/interface.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class StaticRouting final : public Routing {
 public:
  StaticRouting(Node& node);

  Interface * Route(ProtocolPacket &packet) override;
  void Init() override;

  bool AddRoute(const Node &to_node, const Node &via_node);

 private:
  std::map<SimpleAddress, Interface*> mapping_;
};

}  // namespace simulation

#endif  // SARP_STATIC_FORWARDING_STATIC_ROUTING_H_
