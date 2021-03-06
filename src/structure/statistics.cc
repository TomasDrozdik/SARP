//
// statistics.cc
//

#include <iomanip>

#include "structure/position.h"
#include "structure/simulation.h"

namespace simulation {

void Statistics::PrintCsvHeader(std::ostream &os) {
  // clang-format off
  os << "node_density" << ','
     << "mean_node_connectivity" << ','
     << "delivered_packets" << ','
     << "data_packets_lost" << ','
     << "ttl_expired_packets" << ','
     << "cycles_detected" << ','
     << "broken_connection_sends" << ','
     << "routing_result_not_neighbor" << ','
     << "routing_mirror_not_valid" << ','
     << "hops_detected" << ','
     << "routing_overhead_packets_send" << ','
     << "routing_overhead_lost_packets" << ','
     << "rouging_overhead_delivered_packets"<< ','
     << "rouging_overhead_size" << ','
     << "send_event"<< ','
     << "recv_event" << ','
     << "move_event" << ','
     << "update_neighbors_event" << ','
     << "update_routing_event" << ','
     << "update_routing_call" << ','
     << "check_update_routing_call" << ','
     << "routing_record_deletions" << ','
     << "reflexive_routing_result" << ','
     << "routing_table_entries" << ','
     << "routing_periods" << '\n';
  // clang-format on
}

void Statistics::PrintCsv(std::ostream &os, const Network &network) const {
  // clang-format off
  os << DensityOfNodes(network) << ','
     << MeanNodeConnectivity(network) << ','
     << delivered_packets_ << ','
     << data_packets_lost_ << ','
     << ttl_expired_ << ','
     << cycles_detected_ << ','
     << broken_connection_sends_ << ','
     << routing_result_not_neighbor_ << ','
     << routing_mirror_not_valid_ << ','
     << hops_count_ << ','
     << routing_overhead_send_packets_ << ','
     << routing_overhead_lost_packets_ << ','
     << routing_overhead_delivered_packets_ << ','
     << routing_overhead_size_ << ','
     << send_event_ << ','
     << recv_event_ << ','
     << move_event_ << ','
     << update_neighbors_event_ << ','
     << update_routing_event_ << ','
     << update_routing_calls_ << ','
     << check_update_routing_calls_ << ','
     << routing_record_deletion_ << ','
     << reflexive_routing_result_ << ','
     << CountRoutingRecords(network) << ','
     << Routing::GetUpdateConvergence() << '\n';
  // clang-format on
}

void Statistics::Print(std::ostream &os, const Network &network) const {
  // clang-format off
  os << "\n_________STATISTICS_________\n"
     << "node_density: " << DensityOfNodes(network) << " nodes / km^3"
     << "\nmean_node_connectivity: " << MeanNodeConnectivity(network)
     << "\n\n_Traffic_"
     << "\ndelivered_packets: " << delivered_packets_ << "\n\n_Errors_"
     << "\ndata_packets_lost: " << data_packets_lost_
     << "\nttl_expired_packets: " << ttl_expired_
     << "\ncycles_detected: " << cycles_detected_
     << "\nbroken_connection_sends: " << broken_connection_sends_
     << "\nrouting_result_not_neighbor: " << routing_result_not_neighbor_
     << "\nrouting_mirror_not_valid_: " << routing_mirror_not_valid_
     << "\n\n_Routing_"
     << "\nhops_detected: " << hops_count_
     << "\nrouting_overhead_packets_send: " << routing_overhead_send_packets_
     << "\nrouting_overhead_lost_packets: " << routing_overhead_lost_packets_
     << "\nrouging_overhead_delivered_packets: " << routing_overhead_delivered_packets_
     << "\nrouging_overhead_size: " << routing_overhead_size_
     << "\n\n_Events_"
     << "\nsend_event: " << send_event_
     << "\nrecv_event: " << recv_event_
     << "\nmove_event: " << move_event_
     << "\nupdate_neighbors_event: " << update_neighbors_event_
     << "\nupdate_routing_event: " << update_routing_event_
     << "\n\n_Calls_"
     << "\nupdate_routing_call: " << update_routing_calls_
     << "\ncheck_update_routing_call: " << check_update_routing_calls_
     << "\n\n_SARPRoutingRecords_"
     << "\nrouting_record_deletions: " << routing_record_deletion_
     << "\nreflexive_routing_result: " << reflexive_routing_result_
     << "\nrouting_records: " << CountRoutingRecords(network)
     << "\nupdate_convergence: " << Routing::GetUpdateConvergence() << '\n';
  // clang-format on
}

void Statistics::Reset() {
  delivered_packets_ = 0;
  data_packets_lost_ = 0;
  hops_count_ = 0;

  routing_overhead_send_packets_ = 0;
  routing_overhead_lost_packets_ = 0;
  routing_overhead_delivered_packets_ = 0;
  routing_overhead_size_ = 0;

  broken_connection_sends_ = 0;
  cycles_detected_ = 0;
  ttl_expired_ = 0;
  routing_update_from_non_neighbor = 0;
  routing_result_not_neighbor_ = 0;
  routing_mirror_not_valid_ = 0;

  send_event_ = 0;
  recv_event_ = 0;
  move_event_ = 0;
  update_neighbors_event_ = 0;
  update_routing_event_ = 0;

  update_routing_calls_ = 0;
  check_update_routing_calls_ = 0;

  routing_record_deletion_ = 0;
  reflexive_routing_result_ = 0;

  // Also reset the NodeID counter
  Node::ResetID();
}

double Statistics::DensityOfNodes(const Network &network) const {
  if (network.get_nodes().empty()) {
    return 0.0;
  }
  // First find bounding positions
  int max = std::numeric_limits<int>::max();
  Position max_pos(0, 0, 0);
  Position min_pos(max, max, max);
  for (auto &node : network.get_nodes()) {
    max_pos.x = std::max(max_pos.x, node->get_position().x);
    max_pos.y = std::max(max_pos.y, node->get_position().y);
    max_pos.z = std::max(max_pos.z, node->get_position().z);
    min_pos.x = std::min(min_pos.x, node->get_position().x);
    min_pos.y = std::min(min_pos.y, node->get_position().y);
    min_pos.z = std::min(min_pos.z, node->get_position().z);
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
  return static_cast<double>(network.get_nodes().size()) / volume * 1000000;
}

double Statistics::MeanNodeConnectivity(const Network &network) const {
  std::size_t sum = 0;
  for (const auto &node : network.get_nodes()) {
    sum += node->get_neighbors().size();
  }
  return sum / static_cast<double>(network.get_nodes().size());
}

std::size_t Statistics::CountRoutingRecords(const Network &network) {
  std::size_t n = 0;
  for (const auto &node : network.get_nodes()) {
    n += node->get_routing().GetRecordsCount();
  }
  return n;
}

}  // namespace simulation
