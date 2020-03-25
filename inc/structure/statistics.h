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

  static void RegisterDataPacketLoss() { ++data_packets_lost_; }

  static void RegisterHop() { ++hops_count_; }

  static void RegisterRoutingOverheadSend() {
    ++routing_overhead_send_packets_;
  }

  static void RegisterRoutingOverheadLoss() {
    ++routing_overhead_lost_packets_;
  }

  static void RegisterRoutingOverheadDelivered() {
    ++routing_overhead_delivered_packets_;
  }

  static void RegisterRoutingOverheadSize(
      const std::size_t routing_packet_size) {
    routing_overhead_size_ += routing_packet_size;
  }

  static void RegisterBrokenConnectionSend() { ++broken_connection_sends_; }

  static void RegisterDetectedCycle() { ++cycles_detected_; }

  static void RegisterTTLExpire() { ++ttl_expired_; }

  static void RegisterSendEvent() { ++send_event_; }

  static void RegisterRecvEvent() { ++recv_event_; }

  static void RegisterMoveEvent() { ++move_event_; }

  static void RegisterUpdateInterfacesEvent() { ++update_interfaces_event_; }

  static void RegisterUpdateRoutingInterfacesEvent() {
    ++update_routing_interfaces_event_;
  }

  static void RegisterUpdateRoutingEvent() { ++update_routing_event_; }

 private:
  static inline std::size_t delivered_packets_ = 0;
  static inline std::size_t data_packets_lost_ = 0;
  static inline std::size_t hops_count_ = 0;

  static inline std::size_t routing_overhead_send_packets_ = 0;
  static inline std::size_t routing_overhead_lost_packets_ = 0;
  static inline std::size_t routing_overhead_delivered_packets_ = 0;
  static inline std::size_t routing_overhead_size_ = 0;

  static inline std::size_t broken_connection_sends_ = 0;
  static inline std::size_t cycles_detected_ = 0;
  static inline std::size_t ttl_expired_ = 0;

  static inline std::size_t send_event_ = 0;
  static inline std::size_t recv_event_ = 0;
  static inline std::size_t move_event_ = 0;
  static inline std::size_t update_interfaces_event_ = 0;
  static inline std::size_t update_routing_interfaces_event_ = 0;
  static inline std::size_t update_routing_event_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_STATISTICS_H_
