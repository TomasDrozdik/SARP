//
// update_packet.cc
//

#include "update_packet.h"

namespace simulation {

SarpRoutingUpdate::SarpRoutingUpdate(std::unique_ptr<Address> sender_address,
		std::unique_ptr<Address> destination_address,
		SarpRoutingTable &&agregated_table) :
				ProtocolPacket(std::move(sender_address),
						std::move(destination_address)),
				update(std::move(agregated_table)) { }

std::size_t SarpRoutingUpdate::get_size() const {
  return update.get_size();
}

}  // namespace simulation
