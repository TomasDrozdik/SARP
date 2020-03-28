//
// routing.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_H_
#define SARP_DISTANCE_VECTOR_ROUTING_H_

#include <cstdint>
#include <map>
#include <vector>

#include "structure/address.h"
#include "structure/protocol_packet.h"
#include "structure/routing.h"

namespace simulation {

class Routing;

class DistanceVectorRouting final : public Routing {
  friend class DVRoutingUpdate;
  struct Record;

 public:
  using RoutingTableType = std::map<Address, Record>;

  DistanceVectorRouting(Node &node);
  ~DistanceVectorRouting() override = default;

  Node *Route(ProtocolPacket &packet) override;

  // TODO make from_node const
  void Process(ProtocolPacket &packet, Node *from_node) override;

  // Initializes the routing table with the set of neighbors on a given node.
  // All neighbors would be at a 1 hop distance.
  void Init() override;

  // Sends table_ data to all direct neighbors.
  void Update() override;

  void UpdateNeighbors() override;

 private:
  struct Record {
    // Creates new Record, initialize yet unknow metrics with max value.
    Record(Node *via_node, uint32_t metrics);
    Record(const Record &other) = default;

    Node *via_node;
    uint32_t metrics;
  };

  // Updates this with information form other RoutingTable incomming from
  // neighboring node.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const DistanceVectorRouting::RoutingTableType &other,
                     Node *from_node);

  // Routing table
  RoutingTableType table_;
  const uint32_t MAX_METRICS = 15;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_H_
