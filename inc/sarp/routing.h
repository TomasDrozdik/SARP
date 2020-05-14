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
    bool need_generalize = false;
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

  void Dump(std::ostream &os) const;

 private:
  RoutingTable::iterator GetParent(RoutingTable::const_iterator it);

  std::vector<RoutingTable::iterator> GetDirectChildren(RoutingTable::iterator it);

  bool IsRedundant(RoutingTable::const_iterator it, double treshold);

  void UpdatePathToRoot(RoutingTable::const_iterator it);

  RoutingTable::const_iterator CheckAddition(RoutingTable::const_iterator added_item, double treshold);

  void RemoveSubtree(RoutingTable::iterator record);

  RoutingTable::const_iterator AddRecord(Env &env,
      const Address &address, const Cost &cost, Node *via_neighbor);

  void UpdateRouting(Env &env, const UpdateTable &update, Node *from_node);

  void Generalize();

  void GeneralizeRecursive(RoutingTable::iterator it);

  void CreateUpdateMirror();

  RoutingTable table_;

  // Update table is a compacted version of routing table where we omit
  // information about nodes which we go through for given routes.
  // Update packet is send as a reference to precomputed update mirror.
  // Each update mirror table has its unique id counter.
  // This way each packet will be assigned a reference to this mirror and when
  // the id's match and packet reaches it's destination it will procede with the
  // update.
  std::size_t mirror_id_ = 0;
  UpdateTable update_mirror_;

  // Keep history of incomming update packets to compare against.
  std::map<Node *, UpdateTable> update_history_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
