//
// static_routing.h
//

#ifndef SARP_STATIC_FORWARDING_STATIC_ROUTING_H_
#define SARP_STATIC_FORWARDING_STATIC_ROUTING_H_

#include <vector>
#include <unordered_map>

#include "../structure/interface.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class StaticRouting final : public Routing {
 public:
  StaticRouting(Node& node);

  Interface * Route(ProtocolPacket &packet) override;

  // In static routing these are just empty initialization is done by hand with
  // explicit cast and AddRoute method.
  void Init() override;
  void Update() override;

  bool Process(ProtocolPacket &packet,
      Interface *processing_interface) override;

  bool AddRoute(const Node &to_node, const Node &via_node);

 private:
    std::unordered_map<std::unique_ptr<Address>, Interface*,
      AddressHash, AddressComparer> mapping_;
};

}  // namespace simulation

#endif  // SARP_STATIC_FORWARDING_STATIC_ROUTING_H_
