//
// routing.h
//

#ifndef SARP_STRUCTURE_ROUTING_H_
#define SARP_STRUCTURE_ROUTING_H_

#include "address.h"
#include "interface.h"
#include "protocol_packet.h"

namespace simulation {

class Interface;
class Node;
class ProtocolPacket;

class Routing {
 public:
  virtual ~Routing() = 0;
  // Initializes routing self configuration.
  // Should be called just after the network is generated when all nodes are
  // already placed at their positions at the same time for all the nodes.
  virtual void Init() = 0;

  // Starts routing update on node after it was initialized
  virtual void Update() = 0;

  // Finds route for the given packet.
  // RETURNS: nullptr iff packet shouldn't be routed.
  virtual Interface *Route(ProtocolPacket &packet) = 0;

  // Processes given packet which came to processing_interface.
  // RETURNS: true if packet processing should continue, false otherwise.
  virtual bool Process(ProtocolPacket &packet,
      Interface *processing_interface) = 0;

 protected:
  Routing(Node &node);
  Node &node_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ROUTING_H_
