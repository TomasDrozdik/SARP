//
// dv_packet.h
//

#ifndef SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_
#define SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_

#include <memory>
#include <cstdint>

#include "dv_routing.h"
#include "../structure/protocol_packet.h"

namespace simulation {

class DVRoutingUpdate final : public ProtocolPacket {
 public:
  DVRoutingUpdate(std::unique_ptr<Address> sender_address,
      std::unique_ptr<Address> destination_address,
      DistanceVectorRouting::RoutingTableType &table);
  ~DVRoutingUpdate() override = default;

  std::size_t get_size() const override;

  DistanceVectorRouting::RoutingTableType table_copy;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_ROUTING_DV_PACKET_H_