//
// statistics.cc
//

#include "structure/statistics.h"

#include "structure/position.h"

namespace simulation {

std::ostream &Statistics::Print(std::ostream &os, const Network &network) {
  return os << "\n_________STATISTICS_________\n" <<
      "NodeDensity: " << DensityOfNodes(network) << " Nodes / km^3" <<
      "\nMeanNodeConnectivity: " << MeanNodeConnectivity(network) <<
      '\n' <<
      "\n#deliveredPackets: " << delivered_packets_ <<
      "\n#unDeliveredPackets: " << undelivered_packets_ <<
      "\n#brokenConnectionSends: " << broken_connection_sends_ <<
      "\n#hopsDetected: " << hops_count_ <<
      '\n' <<
      "\n#rougingOverheadSendPackets: " << routing_overhead_send_packets_ <<
      "\n#rougingOverheadDeliveredPackets: " << routing_overhead_delivered_packets_ <<
      "\n#rougingOverheadSize: " << routing_overhead_size_ <<
      '\n' <<
      "\n#ttlExpiredPackets: " << ttl_expired_ <<
      "\n#cyclesDetected: " << cycles_detected_ << '\n';
}

double Statistics::DensityOfNodes(const Network &network) {
  // First find bounding positions
  Position max_pos(0,0,0);
  int max = std::numeric_limits<int>::max();
  Position min_pos(max,max,max);
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

double Statistics::MeanNodeConnectivity(const Network &network) {
  std::size_t sum = 0;
  for (auto &node : network.get_nodes()) {
    sum += node->get_active_interfaces().size();
  }
  return sum / static_cast<double>(network.get_nodes().size());
}

}  // namespace simulation
