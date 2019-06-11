//
// simulation.cc
//
#define PRINT

#include "simulation.h"

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
      "\n#timedOutPackets: " << stats.timed_out_packets_ << '\n' <<
      "\n#hopsDetected: " << stats.hops_count_ << '\n' <<
      '\n' <<
      "\n#rougingOverheadSendPackets: " << stats.routing_overhead_send_packets_ <<
      "\n#rougingOverheadDeliveredPackets: " << stats.routing_overhead_delivered_packets_ <<
      "\n#rougingOverheadSize: " << stats.routing_overhead_size_ <<
      '\n' <<
      "\n#cyclesDetected: " << stats.cycles_detected_ << '\n';
}

void Statistics::RegisterDeliveredPacket() {
  ++delivered_packets_;
}

void Statistics::RegisterUndeliveredPacket() {
  ++undelivered_packets_;
}

void Statistics::RegisterTimedOutPacket() {
  ++timed_out_packets_;
}

void Statistics::RegisterHop() {
  ++hops_count_;
}

void Statistics::RegisterRoutingOverheadSend() {
  ++routing_overhead_send_packets_;
}

void Statistics::RegisterRoutingOverheadDelivered() {
  ++routing_overhead_delivered_packets_;
}

void Statistics::RegisterRoutingOverheadSize(
    const std::size_t routing_packet_size) {
  routing_overhead_size_ += routing_packet_size;
}

void Statistics::RegisterDetectedCycle() {
  ++cycles_detected_;
}

size_t Statistics::get_delivered_packets_count() const {
  return delivered_packets_;
}

size_t Statistics::get_undelivered_packets_count() const {
  return undelivered_packets_;
}

size_t Statistics::get_routing_overhead_send() const {
  return routing_overhead_send_packets_;
}

size_t Statistics::get_routing_overhead_delivered() const {
  return routing_overhead_delivered_packets_;
}

size_t Statistics::get_routing_overhead_size() const {
  return routing_overhead_size_;
}

size_t Statistics::get_cycles_detected_count() const {
  return cycles_detected_;
}

Simulation& Simulation::set_properties(std::unique_ptr<Network> network,
    Time duration, uint32_t ttl_limit) {
  if (instance_ == nullptr) {
    instance_ = new Simulation();
  }
  instance_->network_ = std::move(network);
  instance_->statistics_.network_ = instance_->network_.get();
  instance_->simulation_parameters_.duration_ = duration;
  instance_->simulation_parameters_.ttl_limit_ = ttl_limit; return *instance_;
}

Simulation& Simulation::get_instance() {
  if (instance_ == nullptr) {
    instance_ = new Simulation();
  }
  return *instance_;
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
    const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
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