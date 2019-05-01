//
// simulation.cc
//
#define PRINT

#include "simulation.h"

#include <exception>
#include <cstdio>

namespace simulation {

SimulationParameters::SimulationParameters(Time simulation_duration,
    int signal_speed_Mbps) :
        simulation_duration(simulation_duration),
        signal_speed_Mbps(signal_speed_Mbps) { }

void SimulationParameters::Print() const {
  std::printf("\n___SIMULATION_PARAMETERS____\n");
  printf("duration:%zu\n", simulation_duration);
}

Time SimulationParameters::DeliveryDuration(const Node &from, const Node &to,
    const std::size_t packet_size_bytes) const {
  // This calculation is purely based on averagy wifi speed in Mbps and
  // packet size.
  // TODO: form nodes we can obtain also their distance via Position::Distance
  //       this could be used for more precise calculation.
  // Returns time in micro seconds
  return (packet_size_bytes * 8) / signal_speed_Mbps;
}

Simulation& Simulation::set_instance(std::unique_ptr<Network> network,
    Time duration, uint32_t signal_speed_Mbs) {
  if (instance_ != nullptr)
    throw std::logic_error("Simulation is already initialized.");
  instance_ = new Simulation(std::move(network), duration, signal_speed_Mbs);
  return *instance_;
}

Simulation& Simulation::get_instance() {
  if (instance_ == nullptr)
    throw std::logic_error("Simulation is not initialized yet.");
  return *instance_;
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
    const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

Simulation::Simulation(std::unique_ptr<Network> network, Time duration,
    uint32_t signal_speed_Mbs)
        : network_(std::move(network)) {
  statistics_ = std::make_unique<Statistics>(*network_);
  simulation_parameters_ = std::make_unique<SimulationParameters>(duration,
      signal_speed_Mbs);
}

void Simulation::Run(std::vector<std::unique_ptr<Event>> starting_events) {
  for (std::size_t i = 0; i < starting_events.size(); ++i) {
    ScheduleEvent(std::move(starting_events[i]));
  }
#ifdef PRINT
  simulation_parameters_->Print();
  std::printf("\n___________BEGIN____________\n");
  std::printf("time:event:description\n");
#endif
  for (time_ = 0; time_ < simulation_parameters_->simulation_duration;
      ++time_) {
    while (!schedule_.empty() && schedule_.top()->time_ <= time_) {
#ifdef PRINT
      schedule_.top()->Print();
#endif
      schedule_.top()->Execute();
      schedule_.pop();
    }
  }
#ifdef PRINT
  std::printf("____________END_____________\n\n");
  statistics_->Print();
#endif
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->is_absolute_time_) {
    schedule_.push(std::move(event));
  } else {
    event->time_ += this->time_;
    schedule_.push(std::move(event));
  }
}

Time Simulation::get_current_time() const {
  return time_;
}

Statistics& Simulation::get_statistics() {
  return *statistics_;
}
const SimulationParameters& Simulation::get_simulation_parameters() const {
  return *simulation_parameters_;
}

}  // namespace simulation