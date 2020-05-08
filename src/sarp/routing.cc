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

Node *SarpRouting::Route(Env &env, Packet &packet) {
  const Address &destination_address = packet.get_destination_address();
  RoutingTable::const_iterator it = table_.find(destination_address);
  if (it != table_.end()) {
    return it->second.via_node;
  }
  // Find longest common prefix addresses in forwarding table.
  it = table_.cbegin();
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
    env.stats.RegisterReflexiveRoutingResult();
    return nullptr;
  }
  return best_match.via_node;
}

void SarpRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  if (update_packet.IsFresh()) {
    bool change_occured = UpdateRouting(update_packet.update, from_node, env);
    if (change_occured) {
      change_occured_ = true;
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  const auto [it, success] = table_.insert(
      {node_.get_address(),
       {.cost = env.parameters.get_sarp_reflexive_cost(), .via_node = &node_}});
  assert(success);
  // All tables are already initialized with current neighbors from
  // UpdateNeighbors().
  // Now just begin the periodic routing update.
  CheckPeriodicUpdate(env);
}

void SarpRouting::Update(Env &env) {
  // First compact the table before creating a mirror.
  CompactRoutingTable(env);

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

void SarpRouting::UpdateNeighbors(Env &env) {
  // Search for invalid records in routing table.
  for (auto it = table_.cbegin(); it != table_.cend();
       /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_connection_range())) {
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
    const auto [it, success] =
        table_.insert({neighbor->get_address(),
                       {.cost = env.parameters.get_sarp_neighbor_cost(),
                        .via_node = neighbor}});
    if (!success) {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.cost = env.parameters.get_sarp_neighbor_cost();
    }
  }
}

std::pair<bool, bool> SarpRouting::CompactRecords(
    RoutingTable::iterator original, RoutingTable::iterator new_record,
    Env &env) {
  bool did_compact;
  bool did_change;
  if (original == table_.end() || new_record == table_.end()) {
    return {did_change, did_compact};
  }
  if (Cost::AreSimilar(original->second.cost, new_record->second.cost,
                       env.parameters.get_sarp_treshold()) &&
      CommonPrefixLength(original->first, new_record->first) != 0) {
    env.stats.RegisterRoutingRecordDeletion();
    if (original->second.cost.PreferTo(new_record->second.cost)) {
      table_.erase(new_record);
      // Change dit not occure - delete the new, keep the original.
      return {did_change = false, did_compact = true};
    } else {
      table_.erase(original);
      // Change occured - delete the original leave the new.
      return {did_change = true, did_compact = true};
    }
  }
  // Change did occure since records are not similar and we are keeping the
  // new record as well as the original.
  return {did_change = true, did_compact = false};
}

bool SarpRouting::CheckAddition(RoutingTable::iterator it, Env &env) {
  bool change = false;
  // WARNING: position the original and new it accordingly.
  auto [did_change, did_compact] = CompactRecords(FindPrevRecord(it), it, env);
  change |= did_change;
  if (!did_compact) {
    // i.e. the iterator it is still valid, we can compact it with the next one.
    // WARNING: position the original and new it accordingly.
    std::tie(did_change, did_compact) =
        CompactRecords(FindNextRecord(it), it, env);
    change |= did_change;
  }
  return change;
}

bool SarpRouting::AddRecord(const UpdateTable::const_iterator &update_it,
                            Node *via_neighbor, Env &env) {
  const auto &[address, cost] = *update_it;
  Cost actual_cost =
      Cost::AddCosts(cost, env.parameters.get_sarp_neighbor_cost());
  auto [it, success] =
      table_.insert({address, {.cost = actual_cost, .via_node = via_neighbor}});
  if (success) {
    return CheckAddition(it, env);
  }
  // There is already an element with given address.
  // Check which neighbor it goes through.
  if (it->second.via_node == via_neighbor) {
    // If it goes through the same neighbor costs should match.
    if (it->second.cost != actual_cost) {
      // Otherwise record that this route has changed its metrics.
      it->second.cost = actual_cost;
      return CheckAddition(it, env);
    }
  } else {
    // If it goes through different neighbor pick a better route one.
    const Cost &original_cost = it->second.cost;
    if (actual_cost.PreferTo(original_cost)) {
      it->second = {.cost = actual_cost, .via_node = via_neighbor};
      return CheckAddition(it, env);
    }
  }
  return false;
}

bool SarpRouting::UpdateRouting(const UpdateTable &update, Node *from_node,
                                Env &env) {
  bool changed = false;
  for (auto it = update.cbegin(); it != update.cend(); ++it) {
    if (AddRecord(it, from_node, env)) {
      changed = true;
    }
  }
  return changed;
}

SarpRouting::RoutingTable::iterator SarpRouting::FindFirstRecord(
    Node *neighbor) {
  assert(node_.get_neighbors().contains(neighbor));
  auto it = table_.begin();
  while (it != table_.end()) {
    if (it->second.via_node == neighbor) {
      break;
    }
    ++it;
  }
  return it;
}

SarpRouting::RoutingTable::iterator SarpRouting::FindNextRecord(
    RoutingTable::iterator it) {
  assert(it != table_.end());
  const Node *neighbor = it->second.via_node;
  auto i = std::next(it);
  while (i != table_.end()) {
    if (i->second.via_node == neighbor) {
      break;
    }
    ++i;
  }
  return i;
}

SarpRouting::RoutingTable::iterator SarpRouting::FindPrevRecord(
    RoutingTable::iterator it) {
  assert(it != table_.end());
  const Node *neighbor = it->second.via_node;
  auto i = std::prev(it);
  while (i != table_.begin()) {
    if (i->second.via_node == neighbor) {
      break;
    }
    --i;
  }
  if (i->second.via_node != neighbor) {
    // i is table_.begin().
    return table_.end();  // To indicate error.
  }
  return i;
}
void SarpRouting::CleanupTable() {
  for (auto it = table_.begin(); it != table_.end(); /* no inc */) {
    if (it->second.to_delete) {
      it = table_.erase(it);
    } else {
      ++it;
    }
  }
}

// TODO this is in O(n^3)
void SarpRouting::CompactRoutingTable(Env &env) {
  if (!env.parameters.get_sarp_do_compacting()) {
    return;
  }

  for (const auto &neighbor : node_.get_neighbors()) {
    bool cleanup_needed = false;
    for (auto i = FindFirstRecord(neighbor); i != table_.end(); ++i) {
      for (auto j = FindNextRecord(i); j != table_.end(); ++j) {
        // TODO: maybe take this common prefix into account.
        auto common_prefix = CommonPrefixLength(i->first, j->first);
        if (common_prefix == 0) {
          continue;
        }
        // Now compare the distributions.
        if (Cost::AreSimilar(i->second.cost, j->second.cost,
                             env.parameters.get_sarp_treshold())) {
          cleanup_needed = true;
          env.stats.RegisterRoutingRecordDeletion();
          // We cannot erase the iterators here because that would invalidate
          // the other iterator we are not erasing. Therefore we mark those to
          // erase and erase them afterwards.
          if (i->second.cost.PreferTo(j->second.cost)) {
            j->second.to_delete = true;
          } else {
            i->second.to_delete = true;
          }
        }
      }
    }
    if (cleanup_needed) {
      CleanupTable();
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
