//
// simulation.cc
//

#include "simulation.h"

#include <exception>

namespace simulation {

Event::~Event() { }

Event::Event( Time time, bool is_absolute_time) : time_(time),
    is_absolute_time_(is_absolute_time) { }

bool Event::operator<(const Event &other) const {
  return time_ < other.time_;
}

bool Event::EventComparer::operator()(const Event *t1, const Event *t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

Simulation& Simulation::set_instance(
      std::unique_ptr<Network> network) {
  if (instance != nullptr)
    throw std::logic_error("Simulation is already initialized.");
  instance = new Simulation(std::move(network));
  return *instance;
}

Simulation& Simulation::get_instance() {
  if (instance == nullptr)
    throw std::logic_error("Simulation is not initialized yet.");
  return *instance;
}

Simulation::Simulation(std::unique_ptr<Network> network)
    : network_(std::move(network)) {
  for (std::size_t i = 0; i < network_->events_->size(); ++i) {
    ScheduleEvent((*network_->events_)[i].get());
  }
}

void Simulation::Run() {
  for (time_ = 0; time_ < network_->simulation_parameters_->simulation_duration;
      ++time_) {
    while (!schedule_.empty() && schedule_.top()->time_ <= time_) {
      schedule_.top()->execute();
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

Time Simulation::get_current_time() const {
  return time_;
}

Statistics& Simulation::get_statistics() {
  return *network_->statistics_;
}
const SimulationParameters& Simulation::get_simulation_parameters() const {
  return *network_->simulation_parameters_;
}

}  // namespace simulation