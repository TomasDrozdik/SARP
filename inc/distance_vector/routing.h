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

 public:
  using NeighborTableType = std::map<Address, uint32_t>;
  using RoutingTableType = std::map<Node *, NeighborTableType>;

  DistanceVectorRouting(Node &node);

  ~DistanceVectorRouting() override = default;

  Node *Route(ProtocolPacket &packet) override;

  // TODO make from_node const
  void Process(ProtocolPacket &packet, Node *from_node) override;

  // Begin periodic routing update.
  void Init() override;

  // Sends table_ data to all direct neighbors.
  void Update() override;

  // Update the neighbors in the routing table. Remove all neighbor from
  // table_ and add new ones at 1 hop distance.
  void UpdateNeighbors() override;

 private:
  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const DistanceVectorRouting::RoutingTableType &other,
                     Node *from_node);

  static constexpr uint32_t MAX_METRICS = 15;

  RoutingTableType table_;

  // Routing update is a deep copy of table_ computed at the beginning of the
  // update period.
  // Each mirror table has its unique id counter.
  // This way each packet will be assigned a reference to this mirror and when
  // the id's match and packet reaches it's destination it will procede with the
  // update.
  std::size_t mirror_id_ = 0;
  RoutingTableType mirror_table_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_H_
