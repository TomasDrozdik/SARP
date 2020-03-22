//
// routing.h
//

#ifndef SARP_STRUCTURE_ROUTING_H_
#define SARP_STRUCTURE_ROUTING_H_

#include "structure/address.h"
#include "structure/interface.h"
#include "structure/protocol_packet.h"

namespace simulation {

class Interface;
class Node;
class ProtocolPacket;

using Time = std::size_t;

class Routing {
  friend std::ostream &operator<<(std::ostream &os, const Routing &r);

 public:
  // Virtual destructor for abstract class.
  virtual ~Routing() = 0;

  // Initializes routing self configuration with respect to active interfaces on
  // a given node.
  virtual void Init() = 0;

  // Check whether this routing is due to an update (i.e. call to Update())
  // based on last time it was updated and SimulationParameters periodic update
  // period. If not plan the update on that time.
  void CheckPeriodicUpdate();

  // Update the interfaces after the movement of nodes.
  virtual void UpdateInterfaces() = 0;

  // Finds route for the given packet.
  // RETURNS: nullptr iff packet shouldn't be routed otherwise an interface to
  //          route packet through.
  virtual Interface *Route(ProtocolPacket &packet) = 0;

  // Processes given packet which came to processing_interface.
  // RETURNS: true if packet processing should continue, false otherwise.
  virtual bool Process(ProtocolPacket &packet,
                       Interface *processing_interface) = 0;

 protected:
  Routing(Node &node);

  // Starts routing update on node AFTER it was initialized.
  virtual void Update() = 0;

  Time last_update_ = 0;
  Node &node_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ROUTING_H_
