//
// routing.cc
//

#include "sarp/routing.h"

#include <cassert>
#include <cmath>
#include <iomanip>

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
  if (best_match.via_node == &node_) {
    // TODO
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
    UpdateRouting(env, update_packet.update, from_node);
    CheckPeriodicUpdate(env);
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  (void)AddRecord(env, node_.get_address(),
                  env.parameters.get_sarp_reflexive_cost(), &node_);

  // All tables are already initialized with current neighbors from
  // UpdateNeighbors().
  // Now just begin the periodic routing update.
  CheckPeriodicUpdate(env);
}

void SarpRouting::Update(Env &env) {
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
    auto it = FindRecord(neighbor->get_address(), neighbor);
    if (it == table_.end()) {
      AddRecord(env, neighbor->get_address(),
          env.parameters.get_sarp_neighbor_cost(), neighbor);
    } else {
      // If neighbor is already present make sure that it has its metrics set to
      // 1 hop distance.
      it->second.cost = env.parameters.get_sarp_neighbor_cost();
    }
  }
  // TODO search for invalid records in update_history_.
}

SarpRouting::RoutingTable::iterator SarpRouting::FindRecord(const Address &address, Node *neighbor) {
  auto range = table_.equal_range(address);
  for (auto possible_match = range.first; possible_match != range.second; ++possible_match) {
    if (possible_match->second.via_node == neighbor) {
      return possible_match;
    }
  }
  return table_.end();
}

SarpRouting::RoutingTable::iterator SarpRouting::GetParent(RoutingTable::const_iterator record) {
  if (record == table_.end()) {
    return table_.end();
  }
  Address parent_address = record->first;
  if (parent_address.size() <= 1) {
    return table_.end();
  }
  parent_address.pop_back();  // To get the parent address decrease the size
  return FindRecord(parent_address, record->second.via_node);
}

std::vector<SarpRouting::RoutingTable::iterator> SarpRouting::GetDirectChildren(RoutingTable::const_iterator parent) {
  assert(parent->first.size() > 0);

  Address upper_address_bound = parent->first;
  ++upper_address_bound.back();  // Increase the last address component.

  std::vector<RoutingTable::iterator> children;
  auto direct_child_address_size = parent->first.size() + 1;
  for (auto possible_child = table_.lower_bound(parent->first);
       possible_child != table_.upper_bound(upper_address_bound);
       ++possible_child) {
    if (possible_child->first.size() == direct_child_address_size) {
      children.push_back(possible_child);
    }
  }
  return children;
}

bool SarpRouting::IsRedundant(RoutingTable::const_iterator record, double treshold) const {
  assert(record != table_.end());
  return false;
  // TODO find a parent of record and compare to record.
}

void SarpRouting::UpdatePathToRoot(RoutingTable::const_iterator from_record) {
  const auto &[record_address, record_cost_with_neighbor] = *from_record;
  // Cycle through address prefix of all sizes up till 1.
  auto it = from_record;
  for (std::size_t address_index = record_address.size() - 1; address_index > 0;
       --address_index) {
    auto parent = GetParent(it);
    if (parent == table_.end()) {
      // Create a new record with a subaddress of given length.
      Address subaddress(record_address.cbegin(), record_address.cbegin() + address_index);
      // If there is no record of a subaddress that means that this is a new
      // subtree with no childer i.e. its mean and variance are equal to that of
      // its child.
      parent = table_.emplace(subaddress, record_cost_with_neighbor);
    } else {
      parent->second.need_generalize = true;
    }
    it = parent;
  }
}

SarpRouting::RoutingTable::const_iterator SarpRouting::CheckAddition(
    RoutingTable::const_iterator added_item, double treshold) {
  if (IsRedundant(added_item, treshold)) {
    table_.erase(added_item);
    return table_.cend();
  }
  UpdatePathToRoot(added_item);
  auto [inserted_update, success] = new_update_.insert(
      std::make_pair(added_item->first, added_item->second.cost));
  change_occured_ = true;
  //assert(success);
  return added_item;
}

void SarpRouting::MarkRemoved(RoutingTable::iterator record) {
  for (auto it = record; it != table_.end(); it = GetParent(it)) {
    if (GetDirectChildren(it).size() == 1) {
      it->second.to_delete = true;
    } else {
      break;
    }
  }
}

SarpRouting::RoutingTable::const_iterator SarpRouting::AddRecord(Env &env,
    const Address &address, const Cost &cost, Node *via_neighbor) {
  assert(table_.count(address) <= 1 && "Generalized record found in update.");
  auto matching_record = table_.find(address);
  if (matching_record == table_.end()) {
    auto inserted_record = table_.insert(
        {address, {.cost = cost, .via_node = via_neighbor}});
    return CheckAddition(inserted_record, env.parameters.get_sarp_treshold());
  }
  // There is already an element with this address.
  // Check which neighbor matching_record goes through.
  if (matching_record->second.via_node == via_neighbor) {
    // If matching_record goes through the same neighbor costs should match.
    if (matching_record->second.cost != cost) {
      // Otherwise record that this route has changed its metrics.
      matching_record->second.cost = cost;
      return CheckAddition(matching_record, env.parameters.get_sarp_treshold());
    }
  } else {
    // If matching_record goes through different neighbor pick a better route one.
    const Cost &original_cost = matching_record->second.cost;
    if (cost.PreferTo(original_cost)) {
      MarkRemoved(matching_record);
      matching_record->second = {.cost = cost, .via_node = via_neighbor};
      return CheckAddition(matching_record, env.parameters.get_sarp_treshold());
    }
  }
  return table_.cend();
}

void SarpRouting::UpdateRouting(Env &env, const UpdateTable &update,
    Node *from_node) {
  for (auto it = update.cbegin(); it != update.cend(); ++it) {
    const auto &[update_address, update_cost] = *it;
    Cost actual_cost =
        Cost::AddCosts(update_cost, env.parameters.get_sarp_neighbor_cost());
    (void)AddRecord(env, update_address, actual_cost, from_node);
  }
  update_history_[from_node] = update;
  Generalize();
}

void SarpRouting::Generalize() {
  for (auto it = table_.begin(); it != table_.end(); ++it) {
    if (it->first.size() == 1) {
      GeneralizeRecursive(it);
    }
  }
}

void SarpRouting::GeneralizeRecursive(RoutingTable::iterator it) {
  if (it->second.need_generalize == false) {
    return;
  }
  // Recursive call if a child needs to be generalized as well.
  auto children = GetDirectChildren(it);
  for (auto &child : children) {
    if (child->second.need_generalize) {
      GeneralizeRecursive(child);
    }
  }
  // Check wheteher children needs generalizing as well, in that case
  // resursively call on those children.
  std::vector<Cost> children_costs;
  for (auto &child : children) {
    children_costs.push_back(child->second.cost);
  }
  // Update this cost based on all direct children which are already dealt with.
  it->second.cost = Cost(children_costs);
  it->second.need_generalize = false;
}

void SarpRouting::CreateUpdateMirror() {
  ++mirror_id_;
  update_mirror_ = new_update_;
}

void SarpRouting::Dump(std::ostream &os) const {
  os << "Routing table dump: " << node_ << '\n'
     << "address,\t"
     << "cost,\t\t\t\t\t"
     << "via_node, "
     << "generalize, "
     << "to_delete" << '\n';
  for (const auto &record : table_) {
    os << record.first << "\t\t"
       << record.second.cost << "\t\t"
       << *record.second.via_node << "\t\t"
       << record.second.need_generalize << "\t"
       << record.second.to_delete << '\n';
  }
}

}  // namespace simulation

