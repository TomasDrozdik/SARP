//
// statistics.h
//

#ifndef SARP_SIMULATION_STRUCTURE_STATISTICS_H_
#define SARP_SIMULATION_STRUCTURE_STATISTICS_H_

#include <cstddef>

#include "network.h"

namespace simulation {

class Network;

class Statistics {
 public:
  Statistics();
  Statistics(const Network &network);
  ~Statistics() = default;

  void Print() const;
  void RegisterDeliveredPacket();
  void RegisterUndeliveredPacket();
  void RegisterHop();
  void RegisterRoutingOverhead(const std::size_t routing_packet_size);
  void RegisterDetectedCycle();
  double DensityOfNodes() const;
  double MeanNodeConnectivity() const;

  size_t get_delivered_packets_count() const;
  size_t get_undelivered_packets_count() const;
  size_t get_routing_overhead_size() const;
  size_t get_cycles_detected_count() const;

 private:
  const Network &network_;
  std::size_t delivered_packets_ = 0;
  std::size_t undelivered_packets_ = 0;
  std::size_t hops_count_ = 0;
  std::size_t routing_overhead_ = 0;
  std::size_t cycles_detected_ = 0;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_STATISTICS_H_