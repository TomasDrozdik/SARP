//
// simulation.cc
//
#define PRINT

#include "simulation.h"

#include <exception>

namespace simulation {

std::ostream &operator<<(std::ostream &os,
    const SimulationParameters &simulation_parameters) {
  return os << "___SIMULATION_PARAMETERS____\n" <<
      "duration:" << simulation_parameters.simulation_duration << '\n';
}

SimulationParameters::SimulationParameters(Time simulation_duration,
    int signal_speed_Mbps, uint32_t ttl_limit) :
        simulation_duration(simulation_duration),
        signal_speed_Mbps(signal_speed_Mbps),
        ttl_limit(ttl_limit) { }

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

void Simulation::Run(
      std::vector<std::unique_ptr<EventGenerator>> &event_generators) {
  for (auto &generator : event_generators) {
    for (std::unique_ptr<Event> event = std::move(++*generator);
        event != nullptr; event = std::move(++*generator)) {
      ScheduleEvent(std::move(event));
    }
  }
#ifdef PRINT
  std::cout << *simulation_parameters_;
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < simulation_parameters_->simulation_duration;
      ++time_) {
    while (!schedule_.empty() && schedule_.top()->time_ <= time_) {
#ifdef PRINT
      schedule_.top()->Print(std::cout);
#endif
      schedule_.top()->Execute();
      schedule_.pop();
    }
  }
#ifdef PRINT
  std::cout << "____________END_____________\n\n" << *statistics_;
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