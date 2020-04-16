//
// routing.cc
//

#include "sarp/routing.h"

#include <cmath>
#include <limits>
#include <memory>

#include "sarp/update_packet.h"
#include "structure/statistics.h"

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

Node *SarpRouting::Route(ProtocolPacket &packet) {
  const Address &address = packet.get_destination_address();
  const Address upper_address = GetUpperBoundAddress(address);
  std::multimap<Address, std::pair<Record, Node *>> best_matches;
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
  // All tables are already initialized with current neighbors from
  // UpdateNeighbors().
  // Now just begin the periodic routing update.
  CheckPeriodicUpdate();
}

void SarpRouting::Update() {
  // Create new mirror update table as deep copy of current table.
  ++mirror_id_;
  mirror_table_ = table_;
  // Send mirror table to all neighbors.
  for (auto neighbor : node_.get_neighbors()) {
    assert(neighbor != &node_);
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet = std::make_unique<SarpUpdatePacket>(
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

void SarpRouting::UpdateNeighbors() {
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
      auto const [it, success] = table_[neighbor].insert(
          {neighbor->get_address(), Record::DefaultNeighborRecord()});
      assert(success);
    } else {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      table_[neighbor][neighbor->get_address()] =
          Record::DefaultNeighborRecord();
    }
  }
  assert(node_.get_neighbors().size() == table_.size());
}

bool SarpRouting::Record::IsRedundantTo(const SarpRouting::Record &other) {
  // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
  double theta_x1 = cost_sd / (group_size * group_size);
  double theta_x2 = other.cost_sd / (other.group_size * other.group_size);
  double Z = (cost_mean - other.cost_mean) /
             std::sqrt(theta_x1 * theta_x1 + theta_x2 + theta_x2);
  // Now compare with quantile with parameters[https://planetcalc.com/4987]:
  //  Probability 0.975
  //  Variance    1
  //  Mean        0
  constexpr double q = 1.96;
  return std::abs(Z) < q;
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
      this_table_record.AddRecord(record);
      ++table_it;
      ++other_it;
    } else if (this_table_address > address) {
      // The address from other_it is not present in this tree so just insert
      // this new address.
      //
      // This new address however should have its record added to base neighbor
      // record added so that its cost - hop distance increases together with
      // standard deviation of the record.
      Record new_record = Record::DefaultNeighborRecord();
      new_record.AddRecord(record);
      table_it = table.insert(table_it, {address, new_record});
      changed = true;
      ++other_it;
    } else {
      ++table_it;
    }
  }
  return changed;
}

bool SarpRouting::UpdateRouting(const RoutingTableType &update,
                                Node *from_node) {
  bool changed = false;
  auto it = table_.find(from_node);
  // Find out whether from_node is a neighbor in routing table.
  if (it == table_.end()) {
    Statistics::RegisterRoutingUpdateFromNonNeighbor();
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
