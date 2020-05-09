//
// routing.h
//

#ifndef SARP_SARP_ROUTING_H_
#define SARP_SARP_ROUTING_H_

#include <map>

#include "sarp/cost.h"
#include "structure/address.h"
#include "structure/node.h"
#include "structure/packet.h"
#include "structure/routing.h"

namespace simulation {

class SarpRouting final : public Routing {
  friend class CostTests;
  friend class SarpUpdatePacket;

 public:
  struct CostWithNeighbor {
    Cost cost;
    Node *via_node;
  };

  using RoutingTable = std::map<Address, CostWithNeighbor>;
  using UpdateTable = std::map<Address, Cost>;

  SarpRouting(Node &node);

  ~SarpRouting() override = default;

  Node *Route(Env &env, Packet &packet) override;

  void Process(Env &env, Packet &packet, Node *from_node) override;

  // Begin periodic routing update.
  void Init(Env &env) override;

  // Sends table_ data to all direct neighbors.
  void Update(Env &env) override;

  // Update the neighbors in the routing table. Remove all neighbor from
  // table_ and add new ones at 1 hop distance.
  void UpdateNeighbors(Env &env) override;

  std::size_t GetRecordsCount() const override { return table_.size(); }

 private:
  // Compare the two iterators for equality.
  // If they are equal keep the better one according to Cost::PreferTo.
  //
  // RETURNS: true if it did compact the records, false otherwise.
  bool CompactRecords(RoutingTable::iterator it1, RoutingTable::iterator it2,
                      Env &env);

  // Check whether the added record is similar to the previous or the next
  // record with the same via_node.
  // RETURNS: flag indicating change in the table which needs an update.
  bool CheckAddition(RoutingTable::iterator it, Env &env);

  bool AddRecord(const UpdateTable::const_iterator &update_it,
                 Node *via_neighbor, Env &env);

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const UpdateTable &update, Node *from_node, Env &env);

  // Finds the first record of the routing table with via_neighbor set to
  // neighbor.
  RoutingTable::iterator FindFirstRecord(Node *neighbor);

  // Finds the following record of the routing table with teh via_neighbor set
  // to the same via_neighbor as it i.e. it->second.via_neighbor.
  RoutingTable::iterator FindNextRecord(RoutingTable::iterator it);

  // Finds the preceding record of the routing table with teh via_neighbor set
  // to the same via_neighbor as it i.e. it->second.via_neighbor.
  RoutingTable::iterator FindPrevRecord(RoutingTable::iterator it);

  void CreateUpdateMirror();

  RoutingTable table_;

  // Routing update is a agregated version of table_ computed at the beginning
  // of the update period.
  // Each mirror table has its unique id counter.
  // This way each packet will be assigned a reference to this mirror and when
  // the id's match and packet reaches it's destination it will procede with the
  // update.
  std::size_t mirror_id_ = 0;
  UpdateTable update_mirror_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
