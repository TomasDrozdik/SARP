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
  auto update_period = env.parameters.get_general().routing_update_period;
  Time current_time = env.simulation.get_current_time();
  if (next_update_ == current_time) {
    if (change_occured_ || change_notified_) {
      env.stats.RegisterUpdateRoutingCall();
#ifdef UPDATE
      Routing::period_ = current_time / update_period;
#endif
      RequestAllUpdates(env);
      change_occured_ = false;
      change_notified_ = false;
    }
  }
  // Now plan for next update.
  next_update_ = current_time + update_period;
  env.simulation.ScheduleEvent(std::make_unique<UpdateRoutingEvent>(
      next_update_, TimeType::ABSOLUTE, *this));
}

void Routing::RequestUpdate(Env &env, Node *neighbor) {
  env.simulation.ScheduleEvent(std::make_unique<RequestUpdateEvent>(
      1, TimeType::RELATIVE, &node_, neighbor)); // TODO change time
}

void Routing::RequestAllUpdates(Env &env) {
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      continue;
    }
    RequestUpdate(env, neighbor);
  }
}

void Routing::NotifyChange() {
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      continue;
    }
    // TODO do an event for this
    neighbor->get_routing().change_notified_ = true;
  } 
}

}  // namespace simulation
