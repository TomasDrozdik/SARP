//
// statistics.cc
//

#include "statistics.h"

#include <cstdio>

namespace simulation {

Statistics::Statistics(const Network &network) : network_(network) { }

void Statistics::Print() const {
  std::printf("\n_________STATISTICS_________\n");
  std::printf("#deliveredPackets: %zu\n", delivered_packets_);
  std::printf("#unDeliveredPackets: %zu\n", undelivered_packets_);
  std::printf("#rougingOverhead: %zu\n", routing_overhead_);
  std::printf("#cyclesDetected: %zu\n", cycles_detected_);
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

}  // namespace simulation
