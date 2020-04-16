//
// routing.cc
//

#include "distance_vector/routing.h"

#include <algorithm>
#include <cassert>

#include "distance_vector/update_packet.h"
#include "structure/event.h"
#include "structure/simulation.h"
#include "structure/statistics.h"

namespace simulation {

DistanceVectorRouting::DistanceVectorRouting(Node &node) : Routing(node) {}

Node *DistanceVectorRouting::Route(ProtocolPacket &packet) {
  uint32_t min_metrics = std::numeric_limits<uint32_t>::max();
  Node *min_metrics_neighbor = nullptr;

  for (const auto &[neighbor, neighbor_table] : table_) {
    auto it = neighbor_table.find(packet.get_destination_address());
    if (it == neighbor_table.end() || it->second >= MAX_METRICS) {
      continue;
    }
    if (min_metrics > it->second) {
      min_metrics = it->second;
      min_metrics_neighbor = neighbor;
    }
  }
  return min_metrics_neighbor;
}

void DistanceVectorRouting::Process(ProtocolPacket &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  Statistics::RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<DVRoutingUpdate &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured = UpdateRouting(update_packet.mirror_table, from_node);
    if (change_occured) {
      CheckPeriodicUpdate();
    }
  } else {
    Statistics::RegisterInvalidRoutingMirror();
  }
}

void DistanceVectorRouting::Init() {
  // Neighbors were already added in UpdateNeighbors.
  // So just begin the periodic routing update.
  CheckPeriodicUpdate();
}

void DistanceVectorRouting::UpdateNeighbors() {
  // Search routing table for invalid records.
  for (auto it = table_.cbegin(); it != table_.end(); /* no increment */) {
    Node *neighbor = it->first;
    if (node_.IsConnectedTo(*neighbor)) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = table_.erase(it);
    }
  }
  // Now add new neighbors at 1 hop distance.
  for (Node *neighbor : node_.get_neighbors()) {
    auto it = table_.find(neighbor);
    if (it == table_.end()) {
      const auto [it, success] =
          table_[neighbor].emplace(neighbor->get_address(), 1);
      assert(success);
    } else {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      table_[neighbor][neighbor->get_address()] = 1;
    }
  }
  assert(node_.get_neighbors().size() == table_.size());
}

void DistanceVectorRouting::Update() {
  // Create new mirror update table.
  ++mirror_id_;
  mirror_table_ = table_;
  for (auto neighbor : node_.get_neighbors()) {
    assert(neighbor != &node_);
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet = std::make_unique<DVRoutingUpdate>(
        node_.get_address(), neighbor->get_address(), mirror_id_,
        mirror_table_);
    // Register to statistics before we move packet away.
    Statistics::RegisterRoutingOverheadSend();
    Statistics::RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

bool DistanceVectorRouting::UpdateRouting(
    const DistanceVectorRouting::RoutingTableType &update, Node *from_node) {
  bool changed = false;
  // Find out whether from_node is a neighbor in routing table.
  auto it = table_.find(from_node);
  if (it == table_.end()) {
    Statistics::RegisterRoutingUpdateFromNonNeighbor();
    return false;
  }
  NeighborTableType &neighbor_table = it->second;

  constexpr uint32_t distance_to_neighbor = 1;
  assert(neighbor_table.find(from_node->get_address())->second == 1);

  // Check whether the tables from update have some more efficient route than
  // neighbor_table on this node.
  for (const auto &[via_node, via_node_table] : update) {
    assert(via_node != nullptr);

    // HACK: Skip over this node, this is an optimization and hack at the same
    // time:
    //
    // There can be no route update which goes through neighbor to this node_.
    //
    // At the same time since node can have multiple addresses it would have to
    // have each of them listed under all neighbors so that when that address
    // comes from the neighbor as a update this node would know better route to
    // 'itself'.
    if (via_node == &node_) {
      continue;
    }

    for (const auto &[address, metrics] : via_node_table) {
      // Avoid addresses that belong to this node.
      if (node_.get_addresses().contains(address)) {
        continue;
      }
      uint32_t combined_metrics = distance_to_neighbor + metrics;
      if (combined_metrics >= MAX_METRICS) {
        // Count to infinity threshold
        continue;
      }
      auto match = neighbor_table.find(address);
      if (match == neighbor_table.end()) {
        // If there is no record of such to_address add a new record to the
        // table
        const auto [it, success] =
            neighbor_table.emplace(address, combined_metrics);
        assert(success);
        changed = true;
      } else if (match->second != metrics) {
        // If the shortest route goes through the same neighbor, update the
        // value to neighbor metrics.
        match->second = combined_metrics;
        changed = true;
      }
    }
  }
  return changed;
}

}  // namespace simulation
