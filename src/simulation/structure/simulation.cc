//
// simulation.cc
//
#define PRINT

#include "simulation.h"

#include <exception>
#include <cstdio>

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

SimulationParameters::SimulationParameters(Time simulation_duration,
    int signal_speed_Mbps) :
        simulation_duration(simulation_duration),
        signal_speed_Mbps(signal_speed_Mbps) { }

void SimulationParameters::Print() const {
  printf("| SimulationParameters\n|> duration: %zu\n", simulation_duration);
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
  if (instance != nullptr)
    throw std::logic_error("Simulation is already initialized.");
  instance = new Simulation(std::move(network), duration, signal_speed_Mbs);
  return *instance;
}

Simulation& Simulation::get_instance() {
  if (instance == nullptr)
    throw std::logic_error("Simulation is not initialized yet.");
  return *instance;
}

Simulation::Simulation(std::unique_ptr<Network> network, Time duration,
    uint32_t signal_speed_Mbs)
        : network_(std::move(network)) {
  statistics_ = std::make_unique<Statistics>(*network_);
  simulation_parameters_ = std::make_unique<SimulationParameters>(duration,
      signal_speed_Mbs);
  for (std::size_t i = 0; i < network_->events_->size(); ++i) {
    ScheduleEvent((*network_->events_)[i].get());
  }
}

void Simulation::Run() {
#ifdef PRINT
  simulation_parameters_->Print();
  std::printf("\n|> START\n");
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
  std::printf("|> END\n\n");
  statistics_->Print();
#endif
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
  return *statistics_;
}
const SimulationParameters& Simulation::get_simulation_parameters() const {
  return *simulation_parameters_;
}

}  // namespace simulation