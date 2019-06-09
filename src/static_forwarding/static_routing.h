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
  void Init() override;

  bool AddRoute(const Node &to_node, const Node &via_node);

 private:
  struct AddressHash {
    std::size_t operator()(const std::unique_ptr<Address> &addr) const {
      return addr->Hash();
    }
  };

  struct AddressComparer {
    bool operator()(const std::unique_ptr<Address> &a1,
        const std::unique_ptr<Address> &a2) const {
      return *a1 == *a2;
    }
  };

  std::unordered_map<std::unique_ptr<Address>, Interface*,
      AddressHash, AddressComparer> mapping_;
};

}  // namespace simulation

#endif  // SARP_STATIC_FORWARDING_STATIC_ROUTING_H_
