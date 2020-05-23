//
// global_address_update.h
//

#ifndef SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_
#define SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_

#include "network_generator/event_generator.h"
#include "structure/event.h"

namespace simulation {

class SarpGlobalAddressUpdateEvent final : public Event {
 public:
  SarpGlobalAddressUpdateEvent(const Time time, TimeType type,
                               Network &network);
  ~SarpGlobalAddressUpdateEvent() override = default;

  void Execute(Env &env) override;
  std::ostream &Print(std::ostream &os) const override;

 protected:
  // Make priority higher than RoutingUpdate, Send and Recv so that these events
  // have proper neighbor information. On the other hand make it lower than
  // BootEvent since we want to assign addresses to newly booted nodes.
  int get_priority() const override { return 90; }

 private:
  void RecomputeUniqueAddresses(Network &, Position min_pos, Position max_pos);

  Network &network_;
};

class SarpGlobalAddressUpdatePeriodicGenerator final : public EventGenerator {
 public:
  SarpGlobalAddressUpdatePeriodicGenerator(range<Time> time, Time period,
                                           Network &network);

  ~SarpGlobalAddressUpdatePeriodicGenerator() override = default;

  std::unique_ptr<Event> Next() override;

 private:
  range<Time> time_;
  Time period_;
  Network &network_;
  Time virtual_time_;
};

}  // namespace simulation

#endif  // SARP_SARP_GLOBAL_ADDRESS_UPDATE_H_