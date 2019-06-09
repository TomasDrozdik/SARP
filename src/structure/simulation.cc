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
      "duration:" << simulation_parameters.duration_ << '\n';
}

Time SimulationParameters::DeliveryDuration(const Node &from, const Node &to,
    const std::size_t packet_size_bytes) const {
  // TODO: calculate properly
  uint32_t distance = Position::Distance(from.get_connection().position,
      to.get_connection().position);
  return 50;
}

Time SimulationParameters::get_duration() const {
  return duration_;
}

uint32_t SimulationParameters::get_ttl_limit() const {
  return ttl_limit_;
}

std::ostream &operator<<(std::ostream &os, const Statistics stats) {
  return os << "\n_________STATISTICS_________\n" <<
      "#deliveredPackets: " << stats.delivered_packets_ <<
      "\n#unDeliveredPackets: " << stats.undelivered_packets_ <<
      "\n#rougingOverhead: " << stats.routing_overhead_ <<
      "\n#cyclesDetected: " << stats.cycles_detected_ << '\n';
}

void Statistics::RegisterDeliveredPacket() {
  ++Statistics::delivered_packets_;
}

void Statistics::RegisterUndeliveredPacket() {
  ++Statistics::undelivered_packets_;
}

void Statistics::RegisterHop() {
  ++Statistics::hops_count_;
}

void Statistics::RegisterRoutingOverhead(
    const std::size_t routing_packet_size) {
  Statistics::routing_overhead_ += routing_packet_size;
}

void Statistics::RegisterDetectedCycle() {
  ++Statistics::cycles_detected_;
}

size_t Statistics::get_delivered_packets_count() const {
  return delivered_packets_;
}

size_t Statistics::get_undelivered_packets_count() const {
  return undelivered_packets_;
}

size_t Statistics::get_routing_overhead_size() const {
  return routing_overhead_;
}

size_t Statistics::get_cycles_detected_count() const {
  return cycles_detected_;
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
    uint32_t ttl_limit) : network_(std::move(network)) {
  statistics_.network_ = network.get();
  simulation_parameters_.duration_ = duration;
  simulation_parameters_.ttl_limit_ = ttl_limit;
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
  std::cout << simulation_parameters_;
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < simulation_parameters_.duration_;
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
  std::cout << "____________END_____________\n\n" << statistics_;
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
  return statistics_;
}
const SimulationParameters& Simulation::get_simulation_parameters() const {
  return simulation_parameters_;
}

}  // namespace simulation