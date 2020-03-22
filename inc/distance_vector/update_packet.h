//
// update_packet.h
//

#ifndef SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_
#define SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_

#include <cstdint>
#include <memory>

#include "distance_vector/routing.h"
#include "structure/protocol_packet.h"

namespace simulation {

class DVRoutingUpdate final : public ProtocolPacket {
 public:
  DVRoutingUpdate(Address sender_address, Address destination_address,
                  DistanceVectorRouting::RoutingTableType &table);
  ~DVRoutingUpdate() override = default;

  DistanceVectorRouting::RoutingTableType table_copy;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_
