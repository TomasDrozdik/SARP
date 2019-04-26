//
// statistics.cc
//

#include "statistics.h"

namespace simulation {

Statistics::Statistics(const Network &network) : network_(network) { }

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

}  // namespace simulation
