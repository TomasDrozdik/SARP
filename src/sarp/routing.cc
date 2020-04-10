//
// routing.cc
//

#include "sarp/routing.h"

#include <memory>

#include "sarp/update_packet.h"
#include "structure/statistics.h"

namespace simulation {

SarpRouting::SarpRouting(Node &node) : Routing(node) {}

Node *SarpRouting::Route(ProtocolPacket &packet) {
  auto range = table_.equal_range(packet.get_destination_address());
  if (range.first != range.second) {
    return nullptr;
  }
  // TODO: return FindBestMatch(range);
  return nullptr;
}

void SarpRouting::Process(ProtocolPacket &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  Statistics::RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured = UpdateRouting(update_packet.mirror_table, from_node);
    if (change_occured) {
      CheckPeriodicUpdate();
    }
  } else {
    Statistics::RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init() {
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      // Create a 0 metrics record for this node.
       table_.emplace(
          node_.get_address(),
          Record {
             .via_node = &node_,
            .cost_mean = 0,
            .cost_standard_deviation = 1,
            .group_size = 0
          });
    } else {
      // Create a 1 metrics record for a neighbor.
      table_.emplace(
          neighbor->get_address(),
          Record {
            .via_node = neighbor,
            .cost_mean = 1,
            .cost_standard_deviation = 1,
            .group_size = 0,
          });
    }
  }
  // Now begin the periodic routing update.
  CheckPeriodicUpdate();
}

void SarpRouting::UpdateNeighbors() {
  // TODO: invalidate via_node for all lost neighbors.
  for (auto it : table_) {
    if (it.second.via_node && !node_.IsConnectedTo(*it.second.via_node)) {
      it.second.via_node = nullptr;
    }
  }
}

// Agregate routing table and place it in mirror.
// TODO: create separate agregate function - function style.
void SarpRouting::AgregateToMirror() { mirror_table_ = table_; }

void SarpRouting::Update() {
  // Create new mirro update table.
  ++mirror_id_;
  AgregateToMirror();
  // Send agregated table to all neighbors.
  for (auto neighbor : node_.get_neighbors()) {
    // Skip over this node.
    if (neighbor == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet =
        std::make_unique<SarpUpdatePacket>(node_.get_address(),
                                            neighbor->get_address(), mirror_id_,
                                            mirror_table_);
    // Register to statistics before we move packet away.
    Statistics::RegisterRoutingOverheadSend();
    Statistics::RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

bool SarpRouting::UpdateRouting(const RoutingTableType &update,
                                Node *from_node) {
  // TODO
  return false;
}

}  // namespace simulation
