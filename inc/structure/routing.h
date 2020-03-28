//
// routing.h
//

#ifndef SARP_STRUCTURE_ROUTING_H_
#define SARP_STRUCTURE_ROUTING_H_

#include "structure/address.h"
#include "structure/protocol_packet.h"

namespace simulation {

class Node;
class ProtocolPacket;

using Time = std::size_t;

class Routing {
  friend std::ostream &operator<<(std::ostream &os, const Routing &r);

 public:
  // Virtual destructor for abstract class.
  virtual ~Routing() = 0;

  // Initializes routing self configuration with respect to active neighbors on
  // a given node.
  virtual void Init() = 0;

  // Update neighbors after the movement of nodes.
  // Uses node_.get_neighbors().
  virtual void UpdateNeighbors() = 0;

  // Finds route for the given packet.
  // RETURNS: nullptr iff packet shouldn't be routed otherwise a Node to
  //          route packet through.
  virtual Node *Route(ProtocolPacket &packet) = 0;

  // Processes given packet which came from from_node.
  virtual void Process(ProtocolPacket &packet, Node *from_node) = 0;

  // Check whether this routing is due to an update (i.e. call to Update())
  // based on last time it was updated and SimulationParameters periodic update
  // period. If not plan the update on that time.
  void CheckPeriodicUpdate();

 protected:
  Routing(Node &node);

  // Starts routing update on node AFTER it was initialized.
  // Called by CheckPeriodicUpdate in RoutingUpdateEvent.
  virtual void Update() = 0;

  Time next_update_ = 0;
  Node &node_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ROUTING_H_
