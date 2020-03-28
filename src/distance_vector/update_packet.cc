//
// routing_update.cc
//

#include "distance_vector/update_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(Address sender_address,
                                 Address destination_address,
                                 DistanceVectorRouting::RoutingTableType &table)
    : ProtocolPacket(sender_address, destination_address, PacketType::ROUTING,
                     1) {
  // TODO: do this only once per update cycle at the beginning of it and then
  // just send the reference to it.
  //
  // Make deep copy of inner routing table
  for (auto &pair : table) {
    if (pair.second.via_node != nullptr) {  // Exclude invalid neighbors.
      table_copy.emplace(pair.first, pair.second);
    }
  }

  // Set effectively const size_ of this packet.
  size_ = table_copy.size() * sizeof(DistanceVectorRouting::Record);
}

}  // namespace simulation
