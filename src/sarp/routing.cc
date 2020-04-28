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
  if (addr.size() > 1) {
    // If the address has penultimate element increase that one.
    ++addr_upper[addr.size() - 2];
  } else {
    // Otherwise increase last element since there is a limit on element size it
    // is still going to be an upper bound.
    ++addr_upper.back();
  }
  return addr_upper;
}

Node *SarpRouting::Route(Packet &packet) {
  const Address &address = packet.get_destination_address();
  const Address upper_address = GetUpperBoundAddress(address);
  std::multimap<Address, std::pair<CostInfo, Node *>> best_matches;
  // Find best prefix addresses for in all neighbor tables.
  for (const auto &[neighbor, table] : table_) {
    auto lb = table.lower_bound(address);
    auto ub = table.upper_bound(upper_address);
    // Add whole range of with longest common prefix to another map consisting
    // of all such records from all neighbors.
    // This time note particular neighbor to each address and record for later
    // search.
    for (auto it = lb; it != ub; ++it) {
      best_matches.emplace(it->first, std::make_pair(it->second, neighbor));
    }
  }

  // Find highest common prefix among best_matches and pick the shortest route.
  auto lb = best_matches.lower_bound(address);
  auto ub = best_matches.upper_bound(upper_address);
  uint32_t min_cost = std::numeric_limits<uint32_t>::max();
  Node *min_cost_neighbor = nullptr;
  for (auto it = lb; it != ub; ++it) {
    if (it->second.first.cost_mean < min_cost) {
      min_cost = it->second.first.cost_mean;
      min_cost_neighbor = it->second.second;
    }
  }
  return min_cost_neighbor;
}

void SarpRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured = UpdateRouting(update_packet.mirror_table, from_node, env.stats);
    if (change_occured) {
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  // All tables are already initialized with current neighbors from
  // UpdateNeighbors().
  // Now just begin the periodic routing update.
  CheckPeriodicUpdate(env);
}

void SarpRouting::Update(Env &env) {
  // First compact the table before creating a mirror.
  CompactTable(env.stats);
  // Create new mirror update table as deep copy of current table.
  ++mirror_id_;
  mirror_table_ = table_;
  // Send mirror table to all neighbors.
  for (auto neighbor : node_.get_neighbors()) {
    assert(neighbor != &node_);
    // Create update packet.
    std::unique_ptr<Packet> packet = std::make_unique<SarpUpdatePacket>(
        node_.get_address(), neighbor->get_address(), mirror_id_,
        mirror_table_);
    // Register to statistics before we move packet away.
    env.stats.RegisterRoutingOverheadSend();
    env.stats.RegisterRoutingOverheadSize(packet->get_size());
    // Schedule immediate send.
    env.simulation.ScheduleEvent(std::make_unique<SendEvent>(
        1, TimeType::RELATIVE, node_, std::move(packet)));
  }
}

void SarpRouting::UpdateNeighbors(uint32_t connection_range) {
  // Search routing table for invalid records.
  for (auto it = table_.cbegin(); it != table_.end(); /* no increment */) {
    Node *neighbor = it->first;
    if (node_.IsConnectedTo(*neighbor, connection_range)) {
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
      auto const [it, success] = table_[neighbor].insert(
          {neighbor->get_address(), CostInfo::DefaultNeighborCostInfo()});
      assert(success);
    } else {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      table_[neighbor][neighbor->get_address()] =
          CostInfo::DefaultNeighborCostInfo();
    }
  }
  assert(node_.get_neighbors().size() == table_.size());
}

double SarpRouting::CostInfo::ZTest(const CostInfo &r1, const CostInfo &r2) {
  // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
  double z_score = (r1.cost_mean - r2.cost_mean) /
                   std::sqrt(r1.cost_sd * r1.cost_sd + r2.cost_sd * r2.cost_sd);
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

// Merge tables searches for matching addresses and merges their records.
// New addreses are added.
// Takes advantage of sorted maps and applies mergesortlike merge with two
// iterators.
bool SarpRouting::MergeNeighborTables(NeighborTableType &table,
                                      const NeighborTableType &other) {
  bool changed = false;
  auto table_it = table.begin();
  auto other_it = other.cbegin();

  while (other_it != other.cend()) {
    // If we reach the end of the table that means no more matching addresses so
    // just insert the rest of the other table to the table.
    if (table_it == table.end()) {
      table.insert(other_it, other.cend());
      return true;
    }

    auto &[this_table_address, this_table_record] = *table_it;
    const auto &[address, record] = *other_it;

    // Avoid addresses that belong to this node to avoid loops.
    // Reflective traffic is not present.
    if (node_.get_addresses().contains(address)) {
      ++other_it;
      continue;
    }

    if (this_table_address == address) {
      this_table_record.AddCostInfo(record);
      ++table_it;
      ++other_it;
    } else if (this_table_address > address) {
      // The address from other_it is not present in this tree so just insert
      // this new address.
      //
      // This new address however should have its record added to base neighbor
      // record added so that its cost - hop distance increases together with
      // standard deviation of the record.
      CostInfo new_record = CostInfo::DefaultNeighborCostInfo();
      new_record.AddCostInfo(record);
      table_it = table.insert(table_it, {address, new_record});
      changed = true;
      ++other_it;
    } else {
      ++table_it;
    }
  }
  return changed;
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

void SarpRouting::CompactTable(Statistics &stats) {
  for (auto &[neighbor, neighbor_table] : table_) {
    auto it = neighbor_table.begin();
    while (it != neighbor_table.end() &&
           std::next(it) != neighbor_table.end()) {
      const auto &[address, record] = *it;
      const auto &[next_address, next_record] = *std::next(it);

      auto common_prefix = CommonPrefixLength(address, next_address);
      if (common_prefix == 0) {
        ++it;
      }
      if (record.AreSimilar(next_record)) {
        // If they are similar delete the longer one.
        stats.RegisterRoutingRecordDeletion();
        if (record.cost_mean > next_record.cost_mean) {
          it = neighbor_table.erase(it);
        } else {
          // Since we removed next iterator out it is invalidated.
          // Assign next element to it.
          it = neighbor_table.erase(std::next(it));
          // Now to return it to its original place.
          it = std::prev(it);
        }
      } else {
        ++it;
      }
    }
  }
}

bool SarpRouting::UpdateRouting(const RoutingTableType &update,
                                Node *from_node, Statistics &stats) {
  bool changed = false;
  auto it = table_.find(from_node);
  // Find out whether from_node is a neighbor in routing table.
  if (it == table_.end()) {
    stats.RegisterRoutingUpdateFromNonNeighbor();
    return false;
  }
  NeighborTableType &neighbor_table = it->second;

  // Now go through all neighbor tables from update.
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
    if (MergeNeighborTables(neighbor_table, via_node_table)) {
      changed = true;
    }
  }
  return changed;
}

}  // namespace simulation
