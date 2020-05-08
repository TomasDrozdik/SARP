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
    bool to_delete = false;
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

 private:
  // Compare the two iterators for equality.
  // If they are equal keep the better one according to Cost::PreferTo.
  //
  // RETURNS: pair of flags:
  //  .first - inidicates whether the compacting in the routing table of the
  //           original and the new record changed the table and it needs to be
  //           send out as an update.
  //  .second - did we compact the records
  std::pair<bool, bool> CompactRecords(RoutingTable::iterator original,
                                       RoutingTable::iterator new_record,
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

  // Finds the following record of the routing table with teh via_neighbor set
  // to the same via_neighbor as it i.e. it->second.via_neighbor.
  RoutingTable::iterator FindPrevRecord(RoutingTable::iterator it);

  // Delets record labeled to_delete.
  void CleanupTable();

  // Compacts the routing table. Use Cost::AreSimilar function to determine
  // if and entry in map has the similar record as its successor in which case
  // an agregate is created.
  //
  // Table should be compacted once at the beginning of update cycle so that a
  // compacted version is mirrored and sent to neighbors which reduces the
  // overall overhead and processing time.
  void CompactRoutingTable(Env &env);

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
