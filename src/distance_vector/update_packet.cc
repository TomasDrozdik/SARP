//
// routing_update.cc
//

#include "update_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(std::unique_ptr<Address> sender_address,
    std::unique_ptr<Address> destination_address,
    DistanceVectorRouting::RoutingTableType &table) :
        ProtocolPacket(std::move(sender_address),
                       std::move(destination_address),
                       true) {
  // Make deep copy of inner routing table
  for (auto &record : table) {
    if (record.second.via_interface != nullptr) {  // Exclude invalid ifaces.
      table_copy.emplace(record.first->Clone(), record.second);
    }
  }

  // Set effectively const size_ of this packet.
  size_ = table_copy.size() * sizeof(DistanceVectorRouting::Record);
}

}  // namespace simulation
