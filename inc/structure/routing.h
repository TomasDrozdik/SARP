//
// routing.h
//

#ifndef SARP_STRUCTURE_ROUTING_H_
#define SARP_STRUCTURE_ROUTING_H_

#include "structure/address.h"
#include "structure/packet.h"
#include "structure/simulation.h"

namespace simulation {

class Node;
class Packet;
struct Env;
class Statistics;

using Time = std::size_t;

class Routing {
  friend std::ostream &operator<<(std::ostream &os, const Routing &r);

 public:
  // Virtual destructor for abstract class.
  virtual ~Routing() = 0;

  // Initializes routing self configuration with respect to active neighbors on
  // a given node.
  virtual void Init(Env &env) = 0;

  // Update neighbors after the movement of nodes.
  // Uses node_.get_neighbors().
  virtual void UpdateNeighbors(Env &env) = 0;

  // Finds route for the given packet.
  // RETURNS: nullptr iff packet shouldn't be routed otherwise a Node to
  //          route packet through.
  virtual Node *Route(Env &env, Packet &packet) = 0;

  // Processes given packet which came from from_node.
  virtual void Process(Env &env, Packet &packet, Node *from_node) = 0;

  virtual std::size_t GetRecordsCount() const = 0;

  // Check whether this routing is due to an update (i.e. call to Update())
  // based on last time it was updated and Parameters periodic update
  // period. If not plan the update on that time.
  void CheckPeriodicUpdate(Env &env);

 protected:
  Routing(Node &node);

  // Starts routing update on node AFTER it was initialized.
  // Called by CheckPeriodicUpdate in RoutingUpdateEvent.
  virtual void Update(Env &env) = 0;

  Time next_update_ = 0;
  bool change_occured_ = true;
  Node &node_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ROUTING_H_
