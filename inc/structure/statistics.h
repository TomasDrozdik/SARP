//
// simulation.h
//

#ifndef SARP_STRUCTURE_STATISTICS_H_
#define SARP_STRUCTURE_STATISTICS_H_

#include <ostream>

#include "structure/network.h"

namespace simulation {

class Statistics {
 public:
  // Static only class.
  Statistics() = delete;

  static std::ostream &Print(std::ostream &os, const Network &network);
  static double DensityOfNodes(const Network &network);
  static double MeanNodeConnectivity(const Network &network);

  static void RegisterDeliveredPacket() { ++delivered_packets_; }

  static void RegisterUndeliveredPacket() { ++undelivered_packets_; }

  static void RegisterHop() { ++hops_count_; }

  static void RegisterRoutingOverheadSend() {
    ++routing_overhead_send_packets_;
  }

  static void RegisterRoutingOverheadDelivered() {
    ++routing_overhead_delivered_packets_;
  }

  static void RegisterRoutingOverheadSize(
      const std::size_t routing_packet_size) {
    routing_overhead_size_ += routing_packet_size;
  }

  static void RegisterBrokenConnectionsSend() { ++broken_connection_sends_; }

  static void RegisterDetectedCycle() { ++cycles_detected_; }

  static void RegisterTTLExpire() { ++ttl_expired_; }

  static size_t get_delivered_packets_count() { return delivered_packets_; }

  static size_t get_undelivered_packets_count() { return undelivered_packets_; }

  static size_t get_routing_overhead_send() {
    return routing_overhead_send_packets_;
  }

  static size_t get_routing_overhead_delivered() {
    return routing_overhead_delivered_packets_;
  }

  static size_t get_routing_overhead_size() { return routing_overhead_size_; }

  static size_t get_cycles_detected_count() { return cycles_detected_; }

 private:
  static inline std::size_t delivered_packets_ = 0;
  static inline std::size_t undelivered_packets_ = 0;
  static inline std::size_t hops_count_ = 0;
  static inline std::size_t routing_overhead_send_packets_ = 0;
  static inline std::size_t routing_overhead_delivered_packets_ = 0;
  static inline std::size_t routing_overhead_size_ = 0;
  static inline std::size_t broken_connection_sends_ = 0;
  static inline std::size_t cycles_detected_ = 0;
  static inline std::size_t ttl_expired_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_STATISTICS_H_
