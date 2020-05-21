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
#include "structure/simulation.h"

namespace simulation {

class Parameters;

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

  // Begin periodic routing update.
  void Init(Env &env) override;

  // Update the neighbors in the routing table. Remove all neighbors from
  // working table and add new ones at 1 hop distance.
  void UpdateNeighbors(Env &env) override;

  Node *Route(Env &env, Packet &packet) override;

  void Process(Env &env, Packet &packet, Node *from_node) override;

  void SendUpdate(Env &env, Node *neighbor) override;

  std::size_t GetRecordsCount() const override { return working_.size(); }

  void Dump(std::ostream &os) const;

 private:
  static RoutingTable::iterator GetParent(RoutingTable &table, RoutingTable::iterator record);

  static std::vector<RoutingTable::iterator> GetDirectChildren(RoutingTable &table, RoutingTable::iterator record);

  static bool HasRedundantChildren(RoutingTable &table, RoutingTable::iterator record, double compact_treshold, double min_standard_deviation);

  static RoutingTable::iterator RemoveSubtree(RoutingTable &table, RoutingTable::iterator record);

  static void AddRecord(RoutingTable &table,
      const Address &address, const Cost &cost, Node *via_neighbor, Node const * reflexive_via_node);

  static void Generalize(RoutingTable &table, Node const * reflexive_via_node);

  static void GeneralizeRecursive(RoutingTable &table, RoutingTable::iterator record, Node const * reflexive_via_node);

  static void Compact(RoutingTable &table, double compact_treshold, double min_standard_deviation);

  void InsertInitialAddress(Address address, const Cost &cost);

  bool BatchProcessUpdate(const Parameters::Sarp &parameters);

  bool NeedUpdate(const RoutingTable &new_table, double update_treshold) const;

  void CreateUpdateMirror();

  RoutingTable working_;

  UpdateTable update_mirror_;

  // Keep history of incomming update packets to compare against.
  std::size_t neighbor_count_ = 0;
  std::map<Node *, UpdateTable> last_updates_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
