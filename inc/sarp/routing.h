//
// routing.h
//

#ifndef SARP_SARP_ROUTING_H_
#define SARP_SARP_ROUTING_H_

#include <map>

#include "structure/address.h"
#include "structure/node.h"
#include "structure/packet.h"
#include "structure/routing.h"

namespace simulation {

class SarpRouting final : public Routing {
  friend class CostTests;
  friend class SarpUpdatePacket;

 public:
  // Cost which belongs to a route to an address.
  // It is represented by a normal distribution with hop count metrics as a
  // cost.
  // Additionaly record stores the size of the group on the route.
  struct Cost {
    static Cost NoCost() { return Cost{.mean = 0, .sd = 0.1, .group_size = 1}; }

    static Cost DefaultNeighborCost() {
      return Cost{.mean = 1, .sd = 0.1, .group_size = 1};
    }

    static Cost IncreaseByDefaultNeighborCost(const Cost &cost) {
      return CombineCosts(cost, DefaultNeighborCost());
    }

    // Sum of normal distributions.
    static Cost CombineCosts(const Cost &c1, const Cost &c2) {
      return {.mean = c1.mean + c2.mean,
              .sd = c1.sd + c2.sd,
              .group_size = c1.group_size};
      // Don't add the goup size since that is still the same.
    }

    // Function which determines which cost info routing will keep when merging
    // two records with same address but different costs.
    bool PreferTo(const Cost &other) const { return mean < other.mean; }

    static double ZTest(const Cost &r1, const Cost &r2);

    // Declare whether the other normal distribution is 'the same' => redundant
    // to this normal distribution according to Z-test:
    // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
    bool AreSimilar(const Cost &other) const;

    double mean;
    double sd;
    double group_size;  // TODO: In log scale with base 1.1.
  };

  struct CostWithNeighbor {
    Cost cost;
    Node *via_node;
  };

  using RoutingTable = std::map<Address, CostWithNeighbor>;
  using UpdateTable = std::map<Address, Cost>;

  SarpRouting(Node &node);

  ~SarpRouting() override = default;

  Node *Route(Packet &packet) override;

  void Process(Env &env, Packet &packet, Node *from_node) override;

  // Begin periodic routing update.
  void Init(Env &env) override;

  // Sends table_ data to all direct neighbors.
  void Update(Env &env) override;

  // Update the neighbors in the routing table. Remove all neighbor from
  // table_ and add new ones at 1 hop distance.
  void UpdateNeighbors(uint32_t connection_range) override;

 private:
  bool AddRecord(const UpdateTable::const_iterator &update_it,
                 Node *via_neighbor);

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const UpdateTable &update, Node *from_node,
                     Statistics &stats);

  // Compacts the routing table. Use Cost::AreSimilar function to determine
  // if and entry in map has the similar record as its successor in which case
  // an agregate is created.
  //
  // Table should be compacted once at the beginning of update cycle so that a
  // compacted version is mirrored and sent to neighbors which reduces the
  // overall overhead and processing time.
  void CompactRoutingTable(Statistics &stats);

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
