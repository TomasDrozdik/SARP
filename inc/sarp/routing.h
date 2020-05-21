//
// routing.h
//

#ifndef SARP_SARP_ROUTING_H_
#define SARP_SARP_ROUTING_H_

#include "sarp/cost.h"
#include "sarp/sarp_table.h"
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

  std::size_t GetRecordsCount() const override { return table_.Size(); }

  void Dump(std::ostream &os) const;

 private:
  void InsertInitialAddress(Address address, const Cost &cost);

  bool BatchProcessUpdate(const Parameters::Sarp &parameters);

  bool NeedUpdate(const SarpTable &new_table, double update_treshold) const;

  void CreateUpdateMirror();

  SarpTable table_;

  SarpUpdate update_mirror_;

  // Keep history of incomming update packets to compare against.
  std::size_t neighbor_count_ = 0;
  std::map<Node *, SarpUpdate> last_updates_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_H_
