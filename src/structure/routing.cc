//
// routing.cc
//

#include "structure/routing.h"

#include "structure/event.h"
#include "structure/simulation.h"
#include "structure/simulation_parameters.h"
#include "structure/statistics.h"

extern std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Routing &r) {
  return os << "R" << r.node_;
}

Routing::Routing(Node &node) : node_(node) {}

Routing::~Routing() {}

void Routing::CheckPeriodicUpdate() {
  Statistics::RegisterCheckUpdateRoutingCall();
  Time current_time = Simulation::get_instance().get_current_time();
  // Check if next_update_ is planned in future.
  if (next_update_ > current_time) {
    return;
  } else if (next_update_ <= current_time) {
    Statistics::RegisterUpdateRoutingCall();
    Update();
    // Now plan for next update.
    Time last_update = next_update_;
    next_update_ = last_update + config->routing_update_period;
    Simulation::get_instance().ScheduleEvent(
        std::make_unique<UpdateRoutingEvent>(next_update_, TimeType::ABSOLUTE,
                                             *this));
  }
}

}  // namespace simulation
