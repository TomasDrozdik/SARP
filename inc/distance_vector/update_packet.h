//
// update_packet.h
//

#ifndef SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_
#define SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_

#include <cstdint>
#include <memory>

#include "distance_vector/routing.h"
#include "structure/packet.h"

namespace simulation {

class DVRoutingUpdate final : public Packet {
 public:
  DVRoutingUpdate(Address sender_address, Address destination_address,
      DistanceVectorRouting::UpdateTable update)
    : Packet(sender_address, destination_address, PacketType::ROUTING, update.size()),
      update_(update) {}

  ~DVRoutingUpdate() override = default;

  DistanceVectorRouting::UpdateTable &&RetrieveUpdate() {
    return std::move(update_);
  }

 private:
  DistanceVectorRouting::UpdateTable update_;
};

}  // namespace simulation

#endif  // SARP_DISTANCE_VECTOR_UPDATE_PACKET_H_
