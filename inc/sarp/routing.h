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

 public:
  struct Record {
    static Record DefaultNeighborRecord() {
      return Record{.cost_mean = 1, .cost_sd = 0.5, .group_size = 1};
    }

    static Record MergeRecords(const Record &r1, const Record &r2);

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
  // Finds best matching record for given addres i.e. with longest common prefix
  // and best values in record.
  Record *FindBestMatch(const Address &addr);

  // Updates this with information form other RoutingTable incomming from
  // neighbor.
  // RETURNS: true if change has occured, false otherwise
  bool UpdateRouting(const RoutingTableType &update, Node *from_node);

  // Agregates current routing table to minimal size according to
  void AgregateToMirror();


  // Merges two NeighborTableType tables.
  bool MergeNeighborTables(NeighborTableType &table,
                           const NeighborTableType &other);

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
