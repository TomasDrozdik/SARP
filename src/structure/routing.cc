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
  auto update_period = env.parameters.get_routing_update_period();
  Time current_time = env.simulation.get_current_time();
  // Check if next_update_ is planned in future.
  if (next_update_ > current_time) {
    // If the update was not really send in the last period because the check
    // for change did not pass we can send update once in this period as well.
    if (last_update_ < next_update_ - update_period
        && update_requested_) {
      env.stats.RegisterUpdateRoutingCall();
      last_update_ = current_time;
      Update(env);
      update_requested_ = false;
    }
  } else if (next_update_ == current_time) {
    if (change_occured_ || update_requested_) {
      env.stats.RegisterUpdateRoutingCall();
      last_update_ = current_time;
      Update(env);
      change_occured_ = false;
      update_requested_ = false;
    }
    // Now plan for next update.
    next_update_ += update_period;
    env.simulation.ScheduleEvent(std::make_unique<UpdateRoutingEvent>(
        next_update_, TimeType::ABSOLUTE, *this));
  } else {
    assert(false);
  }
}

void Routing::RequestUpdate(Env &env, Node *neighbor) {
  auto &neighbor_routing = neighbor->get_routing();
  neighbor_routing.update_requested_ = true;
  neighbor_routing.CheckPeriodicUpdate(env);
}

}  // namespace simulation
