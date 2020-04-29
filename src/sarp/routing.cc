//
// routing.cc
//

#include "sarp/routing.h"

#include <cassert>
#include <cmath>

#include "sarp/update_packet.h"
#include "structure/event.h"
#include "structure/simulation.h"

namespace simulation {

SarpRouting::SarpRouting(Node &node) : Routing(node) {}

static std::size_t CommonPrefixLength(const Address addr1,
                                      const Address addr2) {
  std::size_t max_size = std::max(addr1.size(), addr2.size());
  for (std::size_t i = 0; i < max_size; ++i) {
    if (addr1[i] != addr2[i]) {
      return i;
    }
  }
  return max_size;
}

Node *SarpRouting::Route(Packet &packet) {
  const Address &destination_address = packet.get_destination_address();
  // Find longest common prefix addresses in forwarding table.
  auto it = table_.cbegin();
  assert(it != table_.cend());
  std::size_t lcp = 0;
  CostWithNeighbor best_match = it->second;
  while (it != table_.cend()) {
    auto cp = CommonPrefixLength(destination_address, it->first);
    if (cp > lcp) {
      lcp = cp;
      best_match = it->second;
    } else if (cp == lcp) {
      if (it->second.cost.PreferTo(best_match.cost)) {
        best_match = it->second;
      }
    } else {  // cp < lcp
      break;
    }
    ++it;
  }
  // TODO
  if (best_match.via_node == &node_) {
    return nullptr;
  }
  return best_match.via_node;
}

void SarpRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured =
        UpdateRouting(update_packet.update, from_node, env.stats);
    if (change_occured) {
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  const auto [it, success] = table_.insert(
      {node_.get_address(), {.cost = Cost::NoCost(), .via_node = &node_}});
  assert(success);
  // All tables are already initialized with current neighbors from
  // UpdateNeighbors().
  // Now just begin the periodic routing update.
  CheckPeriodicUpdate(env);
}

void SarpRouting::Update(Env &env) {
  // First compact the table before creating a mirror.
  // CompactRoutingTable(env.stats);

  // Create new mirror update table as deep copy of current table.
  CreateUpdateMirror();
  // Send mirror table to all neighbors.
  for (auto neighbor : node_.get_neighbors()) {
    if (neighbor == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<Packet> packet = std::make_unique<SarpUpdatePacket>(
        node_.get_address(), neighbor->get_address(), mirror_id_,
        update_mirror_);
    // Register to statistics before we move packet away.
    env.stats.RegisterRoutingOverheadSend();
    env.stats.RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    env.simulation.ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

void SarpRouting::UpdateNeighbors(uint32_t connection_range) {
  // Search for invalid records in routing table.
  for (auto it = table_.cbegin(); it != table_.cend();
       /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, connection_range)) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = table_.erase(it);
    }
  }
  // Now make sure all neighbors are at the 1 hop distance.
  for (Node *neighbor : node_.get_neighbors()) {
    // Skip over this node.
    if (neighbor == &node_) {
      continue;
    }
    const auto [it, success] = table_.insert(
        {neighbor->get_address(),
         {.cost = Cost::DefaultNeighborCost(), .via_node = neighbor}});
    if (!success) {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.cost = Cost::DefaultNeighborCost();
    }
  }
}

double SarpRouting::Cost::ZTest(const Cost &r1, const Cost &r2) {
  // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
  double z_score =
      (r1.mean - r2.mean) / std::sqrt(r1.sd * r1.sd + r2.sd * r2.sd);
  return z_score;
}

bool SarpRouting::Cost::AreSimilar(const SarpRouting::Cost &other) const {
  auto z_score = Cost::ZTest(*this, other);
  // Now compare with quantile with parameters[https://planetcalc.com/4987]:
  //  Probability 0.975
  //  Variance    1
  //  Mean        0
  constexpr double q = 1.96;
  if (z_score == 0) {
    // TODO: rethink this.
    // They are same so don't.
    return false;
  }
  return std::abs(z_score) < q;
}

bool SarpRouting::AddRecord(const UpdateTable::const_iterator &update_it,
                            Node *via_neighbor) {
  bool changed = false;
  const auto &[address, cost] = *update_it;
  Cost actual_cost = Cost::IncreaseByDefaultNeighborCost(cost);
  auto [it, success] =
      table_.insert({address, {.cost = actual_cost, .via_node = via_neighbor}});
  if (!success) {
    // There is already an element with given address.
    // Check which neighbor it goes through.
    if (it->second.via_node == via_neighbor) {
      // Just asign new cost
      it->second.cost = actual_cost;
      changed = true;
    } else {
      const Cost &original_cost = it->second.cost;
      if (actual_cost.PreferTo(original_cost)) {
        // Replace the record for given address.
        it->second = {.cost = actual_cost, .via_node = via_neighbor};
        changed = true;
      }
    }
  }
  return changed;
}

bool SarpRouting::UpdateRouting(const UpdateTable &update, Node *from_node,
                                Statistics &stats) {
  bool changed = false;
  for (auto it = update.cbegin(); it != update.cend(); ++it) {
    if (AddRecord(it, from_node)) {
      changed = true;
    }
  }
  return changed;
}

void SarpRouting::CompactRoutingTable(Statistics &stats) {
  auto it = table_.begin();
  if (it == table_.end()) {
    return;
  }
  while (std::next(it) != table_.end()) {
    const auto &[address, cost_neighbor_pair] = *it;
    const auto &[next_address, next_cost_neighbor_pair] = *std::next(it);

    // TODO: maybe take this common prefix into account.
    auto common_prefix = CommonPrefixLength(address, next_address);
    if (common_prefix == 0) {
      ++it;
    }
    if (cost_neighbor_pair.cost.AreSimilar(next_cost_neighbor_pair.cost)) {
      // If they are similar delete the longer one.
      stats.RegisterRoutingRecordDeletion();
      if (cost_neighbor_pair.cost.mean > next_cost_neighbor_pair.cost.mean) {
        it = table_.erase(it);
      } else {
        // Since we have to remove next iterator out, it is invalidated.
        // Assign next element after removal to it.
        it = table_.erase(std::next(it));
        // Now to return it to its original place.
        it = std::prev(it);
      }
    } else {
      ++it;
    }
  }
}

void SarpRouting::CreateUpdateMirror() {
  ++mirror_id_;
  update_mirror_.clear();
  for (const auto &[address, cost_neighbor_pair] : table_) {
    auto [it, success] =
        update_mirror_.emplace(address, cost_neighbor_pair.cost);
    assert(success);
  }
}

}  // namespace simulation
