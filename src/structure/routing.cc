//
// routing.cc
//

#include "structure/routing.h"

#include "structure/event.h"
#include "structure/simulation.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Routing &r) {
  return os << "R" << r.node_;
}

Routing::Routing(Node &node) : node_(node) {}

Routing::~Routing() {}

void Routing::CheckPeriodicUpdate(Env &env) {
  env.stats.RegisterCheckUpdateRoutingCall();
  Time current_time = env.simulation.get_current_time();
  // Check if next_update_ is planned in future.
  if (next_update_ > current_time) {
    return;
  } else if (next_update_ <= current_time) {
    if (change_occured_) {
      env.stats.RegisterUpdateRoutingCall();
      Update(env);
      change_occured_ = false;
    }
    // Now plan for next update.
    Time last_update = next_update_;
    next_update_ = last_update + env.parameters.get_routing_update_period();
    env.simulation.ScheduleEvent(std::make_unique<UpdateRoutingEvent>(
        next_update_, TimeType::ABSOLUTE, *this));
  }
}

}  // namespace simulation
