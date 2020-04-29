//
// routing.cc
//

#include "sarp/routing.h"

#include <cmath>
#include <limits>
#include <memory>

#include "sarp/update_packet.h"

namespace simulation {

SarpRouting::SarpRouting(Node &node) : Routing(node) {}

static Address GetUpperBoundAddress(const Address &addr) {
  // Assumes octree addresses i.e. max address component is 7.
  // anything below 255 (which is address component - unsigned int) is fine
  // since it won't overflow.
  auto addr_upper = addr;
  addr_upper.push_back(8);
  return addr_upper;
}

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
  auto it = routing_table_.cbegin();
  assert(it != routing_table_.cend());
  std::size_t lcp = 0;
  RoutingTable::mapped_type best_match = it->second;
  while (it != routing_table_.cend()) {
    auto cp = CommonPrefixLength(destination_address, it->first);
    if (cp > lcp) {
      lcp = cp;
      best_match = it->second;
    } else if (cp == lcp) {
      if (it->second.first.PreferTo(best_match.first)) {
        best_match = it->second;
      }
    } else {  // cp < lcp
      break;
    }
    ++it;
  }
  // TODO
  if (best_match.second == &node_) {
    return nullptr;
  }
  return best_match.second;
}

void SarpRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured =
        UpdateRouting(update_packet.update_mirror, from_node, env.stats);
    if (change_occured) {
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  auto [it, success] = routing_table_.insert(
      {node_.get_address(), {CostInfo::NoCost(), &node_}});
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
  for (auto it = routing_table_.cbegin(); it != routing_table_.cend();
       /* no increment */) {
    Neighbor *neighbor = it->second.second;
    if (node_.IsConnectedTo(*neighbor, connection_range)) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = routing_table_.erase(it);
    }
  }
  // Now make sure all neighbors are at the 1 hop distance.
  for (Node *neighbor : node_.get_neighbors()) {
    // Skip over this node.
    if (neighbor == &node_) {
      continue;
    }
    const auto [it, success] = routing_table_.insert(
        {neighbor->get_address(),
         {CostInfo::DefaultNeighborCostInfo(), neighbor}});
    if (!success) {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.first = CostInfo::DefaultNeighborCostInfo();
    }
  }
}

double SarpRouting::CostInfo::ZTest(const CostInfo &r1, const CostInfo &r2) {
  // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
  double z_score =
      (r1.mean - r2.mean) / std::sqrt(r1.sd * r1.sd + r2.sd * r2.sd);
  return z_score;
}

bool SarpRouting::CostInfo::AreSimilar(
    const SarpRouting::CostInfo &other) const {
  auto z_score = CostInfo::ZTest(*this, other);
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
                            Neighbor *via_neighbor) {
  bool changed = false;
  const auto &[address, cost] = *update_it;
  CostInfo actual_cost = CostInfo::IncreaseByDefaultNeighborCost(cost);
  auto [it, success] =
      routing_table_.insert({address, {actual_cost, via_neighbor}});
  if (!success) {
    // There is already an element with given address check which neighbor it
    // goes through.
    if (it->second.second == via_neighbor) {
      // Just asign new cost
      it->second.first = actual_cost;
      changed = true;
    } else {
      const CostInfo &original_cost = it->second.first;
      if (actual_cost.PreferTo(original_cost)) {
        // Replace the record for given address.
        it->second.first = actual_cost;
        it->second.second = via_neighbor;
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
    changed = AddRecord(it, from_node);
  }
  return changed;
}

void SarpRouting::CompactRoutingTable(Statistics &stats) {
  auto it = routing_table_.begin();
  if (it == routing_table_.end()) {
    return;
  }
  while (std::next(it) != routing_table_.end()) {
    const auto &[address, cost_neighbor_pair] = *it;
    const auto &[next_address, next_cost_neighbor_pair] = *std::next(it);

    // TODO: maybe take this common prefix into account.
    auto common_prefix = CommonPrefixLength(address, next_address);
    if (common_prefix == 0) {
      ++it;
    }
    if (cost_neighbor_pair.first.AreSimilar(next_cost_neighbor_pair.first)) {
      // If they are similar delete the longer one.
      stats.RegisterRoutingRecordDeletion();
      if (cost_neighbor_pair.first.mean > next_cost_neighbor_pair.first.mean) {
        it = routing_table_.erase(it);
      } else {
        // Since we have to remove next iterator out, it is invalidated.
        // Assign next element after removal to it.
        it = routing_table_.erase(std::next(it));
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
  for (const auto &[address, cost_neighbor_pair] : routing_table_) {
    auto [it, success] =
        update_mirror_.emplace(address, cost_neighbor_pair.first);
    assert(success);
  }
}

}  // namespace simulation
