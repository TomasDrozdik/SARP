//
// simulation.cc
//

#include "simulation.h"

#include <exception>

namespace simulation {

Event::Event(const Time time, bool is_absolute_time) : time_(time),
    is_absolute_time_(is_absolute_time) { }

bool Event::operator<(const Event &other) const {
  return time_ < other.time_;
}

bool Event::EventComparer::operator()(const Event *t1, const Event *t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

Simulation& Simulation::get_instance(
      const AbstractNetworkFactory &network_factory) {
  if (instance != nullptr)
    throw std::logic_error("Simulation is already initialized.");
  instance = new Simulation(network_factory);
  return *instance;
}

Simulation& Simulation::get_instance() {
  if (instance == nullptr)
    throw std::logic_error("Simulation is not initialized yet.");
  return *instance;
}

Simulation::Simulation(const AbstractNetworkFactory &network_factory) {
  network_ = network_factory.Create();
  for (auto event : *network_->nodes_) {
    ScheduleEvent(event);
  }
}

void Simulation::Run() {
  for (time_ = 0; time_ < network_->simulation_parameters_->simulation_duration;
     ++time_) {
    while (schedule_.top()->time_ <= time_) {
      schedule_.top()->execute();
      // This is raw pointer we also need to delete! See more in header file.
      delete schedule_.top();
      schedule_.pop();
    }
  }
}

void Simulation::ScheduleEvent(Event *event) {
  if (event->is_absolute_time_) {
    schedule_.push(event);
  } else {
    event->time_ += this->time_;
    schedule_.push(event);
  }
}

}  // namespace simulation