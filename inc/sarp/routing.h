//
// routing.h
//

#ifndef SARP_SARP_ROUTING_H_
#define SARP_SARP_ROUTING_H_

#include <map>

#include "structure/address.h"
#include "structure/node.h"
#include "structure/protocol_packet.h"
#include "structure/routing.h"

namespace simulation {

class SarpRouting final : public Routing {
  friend class SarpUpdatePacket;
  struct Record;

 public:
  using RoutingTableType = std::multimap<Address, Record>;

  SarpRouting(Node &node);

  ~SarpRouting() override = default;

  Node *Route(ProtocolPacket &packet) override;

  void Process(ProtocolPacket &packet, Node *from_node) override;

  void Init() override;

  void Update() override;

  void UpdateNeighbors() override;

 private:
  struct Record {
    Node *via_node;
    double cost_mean;
    double cost_standard_deviation;
    double group_size;  // In log scale with base 1.1.
  };

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const RoutingTableType &update, Node *from_node);

  // Agregates current routing table to minimal size according to
  void AgregateToMirror();

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

#endif  // SARP_SARP_ROUTING_H_
