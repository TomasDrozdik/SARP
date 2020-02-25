//
// global_address_update.h
//

#ifndef SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_
#define SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_

#include "../structure/event.h"
#include "../network_generator/event_generator.h"

namespace simulation {

class SarpGlobalAddressUpdateEvent : public Event {
 public:
  SarpGlobalAddressUpdateEvent(const Time time, bool is_absolute_time,
      Network &network);
  ~SarpGlobalAddressUpdateEvent() override = default;

  void Execute() override;
  std::ostream &Print(std::ostream &os) const override;
 private:
  Network &network_;
};

class SarpGlobalAddressUpdatePeriodicGenerator : public EventGenerator {
 public:
  SarpGlobalAddressUpdatePeriodicGenerator(Time start, Time end, Time period,
      Network &network);
  ~SarpGlobalAddressUpdatePeriodicGenerator() override = default;

  std::unique_ptr<Event> operator++() override;

 private: Time period_;
  Network &network_;
  Time virtual_time_;
};

}  // namespace simulation

#endif  // SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_