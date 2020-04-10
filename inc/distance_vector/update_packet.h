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
                  const std::size_t &update_id,
                  const DistanceVectorRouting::RoutingTableType &update);
  ~DVRoutingUpdate() override = default;

  bool IsFresh() const { return original_mirror_id_ == mirror_id_; }

  const DistanceVectorRouting::RoutingTableType &mirror_table;

 private:
  const std::size_t original_mirror_id_;
  const std::size_t &mirror_id_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_
