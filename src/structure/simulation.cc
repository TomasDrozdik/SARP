//
// simulation.cc
//

#define PRINT

#include "simulation.h"

#include <algorithm>
#include <limits>

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
  return 5;
}



std::ostream &operator<<(std::ostream &os, const Statistics stats) {
  return os << "\n_________STATISTICS_________\n" <<
      "NodeDensity: " << stats.DensityOfNodes() << " Nodes / km^3" <<
      "\nMeanNodeConnectivity: " << stats.MeanNodeConnectivity() <<
      '\n' <<
      "\n#deliveredPackets: " << stats.delivered_packets_ <<
      "\n#unDeliveredPackets: " << stats.undelivered_packets_ <<
      "\n#brokenConnectionSends: " << stats.broken_connection_sends_ <<
      "\n#hopsDetected: " << stats.hops_count_ <<
      '\n' <<
      "\n#rougingOverheadSendPackets: " << stats.routing_overhead_send_packets_ <<
      "\n#rougingOverheadDeliveredPackets: " << stats.routing_overhead_delivered_packets_ <<
      "\n#rougingOverheadSize: " << stats.routing_overhead_size_ <<
      '\n' <<
      "\n#ttlExpiredPackets: " << stats.ttl_expired_ <<
      "\n#cyclesDetected: " << stats.cycles_detected_ << '\n';
}

double Statistics::DensityOfNodes() const {
  // First find bounding positions
  Position max_pos(0,0,0);
  int max = std::numeric_limits<int>::max();
  Position min_pos(max,max,max);
  for (auto &node : network_->get_nodes()) {
    max_pos.x = std::max(max_pos.x, node->get_connection().position.x);
    max_pos.y = std::max(max_pos.y, node->get_connection().position.y);
    max_pos.z = std::max(max_pos.z, node->get_connection().position.z);
    min_pos.x = std::min(min_pos.x, node->get_connection().position.x);
    min_pos.y = std::min(min_pos.y, node->get_connection().position.y);
    min_pos.z = std::min(min_pos.z, node->get_connection().position.z);
  }
  int dx = max_pos.x - min_pos.x;
  if (dx == 0) {
    dx = 1;
  }
  int dy = max_pos.y - min_pos.y;
  if (dy == 0) {
    dy = 1;
  }
  int dz = max_pos.z - min_pos.z;
  if (dz == 0) {
    dz = 1;
  }
  std::size_t volume = dx * dy * dz;
  return static_cast<double>(network_->get_nodes().size()) / volume * 1000000;
}

double Statistics::MeanNodeConnectivity() const {
  std::size_t sum = 0;
  for (auto &node : network_->get_nodes()) {
    sum += node->get_active_interfaces().size();
  }
  return sum / static_cast<double>(network_->get_nodes().size());
}


Simulation& Simulation::set_properties(std::unique_ptr<Network> network,
    Time duration, uint32_t ttl_limit) {
  if (instance_ == nullptr) {
    instance_ = new Simulation();
  }
  instance_->network_ = std::move(network);
  instance_->statistics_.set_network(instance_->network_.get());
  instance_->simulation_parameters_.duration_ = duration;
  instance_->simulation_parameters_.ttl_limit_ = ttl_limit;
  return *instance_;
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
    for (std::unique_ptr<Event> event = ++(*generator);
        event != nullptr; event = ++(*generator)) {
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
  std::cout << "____________END_____________\n\n" << statistics_;
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->IsRelativeTime()) {
    event->time_ += this->time_;
  }
  schedule_.push(std::move(event));
}

}  // namespace simulation
