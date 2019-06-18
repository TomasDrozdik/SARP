//
// dv_packet.cc
//

#include "dv_packet.h"

namespace simulation {

DVRoutingUpdate::DVRoutingUpdate(std::unique_ptr<Address> sender_address,
    std::unique_ptr<Address> destination_address,
    DistanceVectorRouting::RoutingTableType &table) :
        ProtocolPacket(std::move(sender_address),
            std::move(destination_address), true) {
  // Make deep copy of inner routing table
  for (auto &record : table) {
    table_copy.emplace(record.first->Clone(), record.second);
  }
}

std::size_t DVRoutingUpdate::get_size() const {
  return table_copy.size() * sizeof(DistanceVectorRouting::Record);
}

}  // namcespace simulation