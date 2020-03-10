//
// update_packet.cc
//

#include "sarp/update_packet.h"

namespace simulation {

SarpRoutingUpdate::SarpRoutingUpdate(std::unique_ptr<Address> sender_address,
		std::unique_ptr<Address> destination_address,
		SarpRoutingTable &&agregated_table) :
				ProtocolPacket(std::move(sender_address),
						           std::move(destination_address),
                       true),
				update(std::move(agregated_table)) {
  this->size_ = update.get_size();
}

}  // namespace simulation
