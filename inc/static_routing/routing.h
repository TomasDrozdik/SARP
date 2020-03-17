//
// routing.h
//

#ifndef SARP_STATIC_ROUTING_ROUTING_H_
#define SARP_STATIC_ROUTING_ROUTING_H_

#include <map>
#include <vector>

#include "structure/interface.h"
#include "structure/routing.h"
#include "structure/simulation.h"

namespace simulation {

class StaticRouting final : public Routing {
 public:
  StaticRouting(Node &node);

  Interface *Route(ProtocolPacket &packet) override;

  // In static routing these are just empty initialization is done by hand with
  // explicit cast and AddRoute method.
  void Init() override {}
  void Update() override {}
  void UpdateInterfaces() override {}

  // Since there is no dynamic routing update, every packet should be processed.
  bool Process(ProtocolPacket &, Interface *) override { return true; }

  // Staticly add route for given node. No masking is present, an exact match
  // to to_node has to be found in mapping for successful Route.
  //
  // Moreover via_node argument has to be a node which is connected to
  // this->node_ via an interface otherwise Route cannot find an interface to
  // route the packet through.
  //
  // Due to these requirements static routing is bound to break when a mobility
  // is introduced to simulation.
  bool AddRoute(const Node &to_node, const Node &via_node);

 private:
  std::map<Address, Interface *> mapping_;
};

}  // namespace simulation

#endif  // SARP_STATIC_ROUTING_ROUTING_H_
