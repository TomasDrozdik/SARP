//
// update_packet.h
//

#ifndef SARP_SARP_UPDATE_PACKET_H_
#define SARP_SARP_UPDATE_PACKET_H_

#include <cstdint>
#include <memory>

#include "sarp/routing.h"
#include "structure/packet.h"

namespace simulation {

class SarpUpdatePacket final : public Packet {
 public:
  SarpUpdatePacket(Address sender_address, Address destination_address,
                   const std::size_t &id,
                   const SarpRouting::RoutingTableType &update);

  ~SarpUpdatePacket() override = default;

  bool IsFresh() const { return original_mirror_id_ == mirror_id_; }

  const SarpRouting::RoutingTableType &mirror_table;

 private:
  const std::size_t original_mirror_id_;
  const std::size_t &mirror_id_;
};

}  // namespace simulation

#endif  // SARP_SARP_UPDATE_PACKET_H_
