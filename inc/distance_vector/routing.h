//
// routing.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_H_
#define SARP_DISTANCE_VECTOR_ROUTING_H_

#include <cstdint>
#include <map>
#include <vector>

#include "structure/address.h"
#include "structure/packet.h"
#include "structure/routing.h"

namespace simulation {

class Routing;

class DistanceVectorRouting final : public Routing {
  friend class DVRoutingUpdate;

 public:
  using Metrics = uint32_t;

  struct MetricsWithNeighbor {
    Metrics metrics;
    Node *via_node;
  };

  using RoutingTable = std::map<Address, MetricsWithNeighbor>;
  using UpdateTable = std::map<Address, Metrics>;

  DistanceVectorRouting(Node &node);

  ~DistanceVectorRouting() override = default;

  Node *Route(Packet &packet) override;

  // TODO make from_node const
  void Process(Env &env, Packet &packet, Node *from_node) override;

  // Begin periodic routing update.
  void Init(Env &env) override;

  // Sends table_ data to all direct neighbors.
  void Update(Env &env) override;

  // Update the neighbors in the routing table. Remove all neighbor from
  // table_ and add new ones at 1 hop distance.
  void UpdateNeighbors(uint32_t connection_range) override;

 private:
  static constexpr Metrics MAX_METRICS = 15;
  static constexpr Metrics NEIGHBOR_METRICS = 1;
  static constexpr Metrics MIN_METRICS = 0;

  bool AddRecord(UpdateTable::const_iterator update_it, Node *via_neighbor);

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const UpdateTable &update, Node *from_node,
                     Statistics &stats);

  void CreateUpdateMirror();

  RoutingTable table_;

  // Routing update is a deep copy of table_ computed at the beginning of the
  // update period.
  // Each mirror table has its unique id counter.
  // This way each packet will be assigned a reference to this mirror and when
  // the id's match and packet reaches it's destination it will procede with the
  // update.
  std::size_t mirror_id_ = 0;
  UpdateTable update_mirror_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_H_
