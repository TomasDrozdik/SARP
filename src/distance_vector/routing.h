//
// routing.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_H_
#define SARP_DISTANCE_VECTOR_ROUTING_H_

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "../structure/address.h"
#include "../structure/interface.h"
#include "../structure/protocol_packet.h"
#include "../structure/routing.h"
#include "../structure/simulation.h"

namespace simulation {

class Routing;

class DistanceVectorRouting final : public Routing {
 friend class DVRoutingUpdate;
 struct Record;
 public:
  using RoutingTableType = std::unordered_map<std::unique_ptr<Address>, Record,
      AddressHash, AddressComparer>;

  DistanceVectorRouting(Node &node);
  ~DistanceVectorRouting() override = default;

  Interface *Route(ProtocolPacket &packet) override;

  bool Process(ProtocolPacket &packet,
      Interface *processing_interface) override;

  // Initializes the routing table with the set of interfaces on a given node
  // All the active interafaces would be at a 1 hop distance
  void Init() override;

  // Sends table_ data to all direct neighbors
  void Update() override;

  void UpdateInterfaces() override;

 private:
  struct Record {
      // Creates new Record, fills all interfaces in data and initializes yet
      // unknow with max value
      Record(Interface *via_interface, uint32_t metrics);
      Record(const Record &other) = default;

      // Pointer to active_interfaces_routingPOV_
      Interface *via_interface;
      uint32_t metrics;
  };


  // Updates this with information form other RoutingTable incomming from
  // processing interface
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const DistanceVectorRouting::RoutingTableType &other,
      Interface *processing_interface);

  // Routing table
  RoutingTableType table_;
  const uint32_t MAX_METRICS = 15;

  // Make updates periodical
  Time update_period_ = 1000;
  Time last_update_ = 0;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_H_
