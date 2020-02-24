//
// global_address_update.cc
//

#include "global_address_update.h"

#include "address.h"

namespace simulation {

SarpGlobalAddressUpdateEvent::SarpGlobalAddressUpdateEvent(
		const Time time, bool is_absolute_time, Network &network) :
				Event(time, is_absolute_time), network_(network) { }

void SarpGlobalAddressUpdateEvent::Execute() {
	SarpAddress::AssignAddresses(network_);
}

std::ostream &SarpGlobalAddressUpdateEvent::Print(std::ostream &os) const {
  return os << time_ << ":sarp_address_update:" << '\n';
}

SarpGlobalAddressUpdatePeriodicGenerator::SarpGlobalAddressUpdatePeriodicGenerator(
		Time start, Time end, Time period, Network &network) :
        EventGenerator(start, end), period_(period), network_(network),
        virtual_time_(start) { }

std::unique_ptr<Event> SarpGlobalAddressUpdatePeriodicGenerator::operator++() {
  if (virtual_time_ >= end_) {
    return nullptr;
  }
	virtual_time_ += period_;
	return std::make_unique<SarpGlobalAddressUpdateEvent>(virtual_time_, true, network_);
}

}  // namespace simulation
