//
// routing.h
//

#ifndef SARP_SARP_ROUTING_H_
#define SARP_SARP_ROUTING_H_

#include "routing_table.h"
#include "../structure/address.h"
#include "../structure/interface.h"
#include "../structure/protocol_packet.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class SarpRouting final : public Routing {
 public:
//  using RoutingTableType = std::unordered_map<std::unique_ptr<Address>, Record,
//      AddressHash, AddressComparer>;

  SarpRouting(Node &node);
  ~SarpRouting() override = default;

  Interface *Route(ProtocolPacket &packet) override;

  bool Process(ProtocolPacket &packet,
      Interface *processing_interface) override;

  void Init() override;

  void Update() override;

  void UpdateInterfaces() override;

 private:


  // Updates this with information form other RoutingTable incomming from
  // processing interface
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const SarpRoutingTable &update,
      Interface *processing_interface);

  // Routing table
  SarpRoutingTable table_;

  // Make updates periodical
  Time update_period_ = 1000;
  Time last_update_ = 0;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
