//
// statistics.cc
//

#include "structure/statistics.h"

#include "structure/position.h"

namespace simulation {

std::ostream &Statistics::Print(std::ostream &os, const Network &network) {
  return os << "\n_________STATISTICS_________\n"
            << "NodeDensity: " << DensityOfNodes(network) << " Nodes / km^3"
            << "\nmean_node_connectivity: " << MeanNodeConnectivity(network)
            << "\n\n_Traffic_"
            << "\n#delivered_packets: " << delivered_packets_ << "\n\n_Errors_"
            << "\n#data_packets_lost: " << data_packets_lost_
            << "\n#ttl_expired_packets: " << ttl_expired_
            << "\n#cycles_detected: " << cycles_detected_
            << "\n#broken_connection_sends: " << broken_connection_sends_
            << "\n#routing_result_not_neighbor: "
            << routing_result_not_neighbor_
            << "\n#routing_mirror_not_valid_: " << routing_mirror_not_valid_
            << "\n\n_Routing_"
            << "\n#hops_detected: " << hops_count_
            << "\n#routing_overhead_packets_send: "
            << routing_overhead_send_packets_
            << "\n#routing_overhead_lost_packets: "
            << routing_overhead_lost_packets_
            << "\n#rouging_overhead_delivered_packets: "
            << routing_overhead_delivered_packets_
            << "\n#rouging_overhead_size: " << routing_overhead_size_
            << "\n\n_Events_"
            << "\n#send_event: " << send_event_
            << "\n#recv_event: " << recv_event_
            << "\n#move_event: " << move_event_
            << "\n#update_neighbors_event: " << update_neighbors_event_
            << "\n#update_routing_event: " << update_routing_event_
            << "\n\n_Calls_"
            << "\n#update_routing_call: " << update_routing_calls_
            << "\n#check_update_routing_call: " << check_update_routing_calls_
            << "\n\n_SARPRoutingRecords_"
            << "\nrouting_record_deletions: " << routing_record_deletion_;
}

double Statistics::DensityOfNodes(const Network &network) {
  assert(!network.get_nodes().empty());
  // First find bounding positions
  int max = std::numeric_limits<int>::max();
  Position max_pos(0, 0, 0);
  Position min_pos(max, max, max);
  for (auto &node : network.get_nodes()) {
    max_pos.x = std::max(max_pos.x, node.get_position().x);
    max_pos.y = std::max(max_pos.y, node.get_position().y);
    max_pos.z = std::max(max_pos.z, node.get_position().z);
    min_pos.x = std::min(min_pos.x, node.get_position().x);
    min_pos.y = std::min(min_pos.y, node.get_position().y);
    min_pos.z = std::min(min_pos.z, node.get_position().z);
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

double Statistics::MeanNodeConnectivity(const Network &network) {
  std::size_t sum = 0;
  for (auto &node : network.get_nodes()) {
    sum += node.get_neighbors().size();
  }
  return sum / static_cast<double>(network.get_nodes().size());
}

}  // namespace simulation
