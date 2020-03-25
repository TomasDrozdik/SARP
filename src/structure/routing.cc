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

void Routing::CheckPeriodicUpdate() {
  if (!SimulationParameters::DoPeriodicRoutingUpdate()) {
    return;
  }
  Time current_time = Simulation::get_instance().get_current_time();
  Time due_update = current_time - last_update_;
  Time update_period = SimulationParameters::get_periodic_update_period();
  if (due_update > update_period) {
    last_update_ = current_time;
    // Do an instantanious Update() without UpdateEvent.
    Update();
  } else {
    // Plan the update at the given period.
    Time time_to_period = current_time % update_period;
    if (time_to_period == 0) {
      time_to_period = update_period;
    }
    last_update_ = current_time + time_to_period;  // now in future
    Simulation::get_instance().ScheduleEvent(
        std::make_unique<UpdateRoutingEvent>(last_update_, TimeType::ABSOLUTE,
                                             *this));
  }
}

}  // namespace simulation
