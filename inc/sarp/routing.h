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
  using NeighborTableType = std::map<Address, Record>;
  using RoutingTableType = std::map<Node *, NeighborTableType>;

  SarpRouting(Node &node);

  ~SarpRouting() override = default;

  Node *Route(ProtocolPacket &packet) override;

  void Process(ProtocolPacket &packet, Node *from_node) override;

  // Begin periodic routing update.
  void Init() override;

  // Sends table_ data to all direct neighbors.
  void Update() override;

  // Update the neighbors in the routing table. Remove all neighbor from
  // table_ and add new ones at 1 hop distance.
  void UpdateNeighbors() override;

 private:
  // Record which belongs to a route to an address.
  // It is represented by a normal distribution with hop count metrics as a
  // cost.
  // Additionaly record stores the size of the group on the route.
  struct Record {
    static Record DefaultNeighborRecord() {
      return Record{.cost_mean = 1, .cost_sd = 0.5, .group_size = 1};
    }

    // TODO:
    // Agregates two normal distributions together.
    // Can make use of group size and do a weighted average.
    //
    // static Record MergeRecords(const Record &r1, const Record &r2);

    // Declare whether the other normal distribution is 'the same' => redundant
    // to this normal distribution according to Z-test:
    // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
    bool IsRedundantTo(const Record &other);

    // Sum of normal distributions.
    void AddRecord(const Record &other) {
      cost_mean += other.cost_mean;
      cost_sd += other.cost_sd;
      // Don't add the goup size since that is still the same.
    }

    double cost_mean;
    double cost_sd;
    double group_size;  // TODO: In log scale with base 1.1.
  };

  // Merges neighbor table other into table.
  // Return true if table has changed, false otherwise.
  bool MergeNeighborTables(NeighborTableType &table,
                           const NeighborTableType &other);

  // TODO:
  // Compacts the routing table. Use Record::AreSimilar function to determine if
  // and entry in map has the similar record as its successor in which case an
  // agregate is created.
  //
  // Table should be compacted once at the beginning of update cycle so that a
  // compacted version is mirrored and sent to neighbors which reduces the
  // overall overhead and processing time.
  //
  // void CompactTable();

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const RoutingTableType &update, Node *from_node);

  // Map of routing tables to each neighbor.
  RoutingTableType table_;

  // Routing update is a agregated version of table_ computed at the beginning
  // of the update period.
  // Each mirror table has its unique id counter.
  // This way each packet will be assigned a reference to this mirror and when
  // the id's match and packet reaches it's destination it will procede with the
  // update.
  std::size_t mirror_id_ = 0;
  RoutingTableType mirror_table_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
