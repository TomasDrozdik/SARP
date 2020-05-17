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

  // TODO
  auto i = working_.find(destination_address);
  if (i != working_.end()) {
    return i->second.via_node;
  }

  // Find longest common prefix addresses in forwarding table.
  auto it = working_.cbegin();
  assert(it != working_.cend());
  std::size_t lcp = 0;
  auto best_match = it->second;
  while (it != working_.cend()) {
    auto cp = CommonPrefixLength(destination_address, it->first);
    if (cp > lcp) {
      lcp = cp;
      best_match = it->second;
    } else if (cp == lcp) {
      if (it->second.cost.PreferTo(best_match.cost) && it->second.via_node != &node_) {
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
    last_updates_[from_node] = update_packet.update;
    if (last_updates_.size() == neighbor_count_) {
      change_occured_ = BatchProcessUpdate(
          env.parameters.get_sarp_neighbor_cost(), 
          env.parameters.get_sarp_reflexive_cost(),
          env.parameters.get_sarp_treshold());
      CheckPeriodicUpdate(env);
    } else {
      change_occured_ = true;
      CheckPeriodicUpdate(env);
    }
  } else {
    env.stats.RegisterInvalidRoutingMirror();
  }
}

void SarpRouting::Init(Env &env) {
  for (const auto &address : node_.get_addresses()) {
    InsertInitialAddress(address, env.parameters.get_sarp_reflexive_cost());
  }
  // Begin the periodic routing update.
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
    // Schedule immediate recieve on neighbor to bypass Node::Send() which calls
    // Routing::Route which is not desired.
    env.simulation.ScheduleEvent(std::make_unique<RecvEvent>(
        1, TimeType::RELATIVE, node_, *neighbor, std::move(packet)));
  }
}

void SarpRouting::UpdateNeighbors(Env &env) {
  // Search for invalid records in routing table.
  for (auto it = working_.cbegin(); it != working_.cend();
       /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_connection_range())) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = working_.erase(it);
    }
  }
  // Now clear the update history of invalid records.
  neighbor_count_ = node_.get_neighbors().size() - 1;  // -1 for reflexive node
  for (auto it = last_updates_.cbegin();
       it != last_updates_.cend();
       /* no increment */) {
    Node *neighbor = it->first;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_connection_range())) {
      assert(node_.get_neighbors().contains(neighbor));
      ++it;
    } else {
      assert(!node_.get_neighbors().contains(neighbor));
      it = last_updates_.erase(it);
    }
  }
}

void SarpRouting::Dump(std::ostream &os) const {
  os << "Routing table dump: " << node_ << '\n'
     << "address,\t"
     << "cost,\t\t\t\t\t"
     << "via_node, "
     << "generalize\n";
  for (const auto &record : working_) {
    os << record.first << "\t\t"
       << record.second.cost << "\t\t"
       << *record.second.via_node << "\n";
  }
}

SarpRouting::RoutingTable::iterator SarpRouting::GetParent(RoutingTable &table, RoutingTable::iterator record) {
  if (record == table.end()) {
    return table.end();
  }
  Address parent_address = record->first;
  if (parent_address.size() <= 1) {
    return table.end();
  }
  // To get the parent address remove last component.
  parent_address.pop_back();
  return table.find(parent_address);
}

std::vector<SarpRouting::RoutingTable::iterator> SarpRouting::GetDirectChildren(RoutingTable &table, RoutingTable::iterator parent) {
  assert(parent->first.size() > 0);
  auto lower_bound = parent;
  Address upper_address_bound = parent->first;
  upper_address_bound.back() += 1;  // Increase the last address component.
  auto upper_bound = table.upper_bound(upper_address_bound);
  // Now search for records with child address length in the lower - upper bound
  // range.
  auto direct_child_address_size = parent->first.size() + 1;
  std::vector<RoutingTable::iterator> children;
  for (auto possible_child = lower_bound; possible_child != upper_bound; ++possible_child) {
    if (possible_child->first.size() == direct_child_address_size) {
      children.push_back(possible_child);
    }
  }
  return children;
}

bool SarpRouting::IsRedundant(RoutingTable &table, RoutingTable::iterator record, double treshold) {
  assert(record != table.end());
  auto parent = GetParent(table, record);
  if (parent == table.end()) {
    return false;
  }
  auto zscore = parent->second.cost.ZScore(record->second.cost);
  return zscore > treshold;
}

SarpRouting::RoutingTable::iterator SarpRouting::RemoveSubtree(
    RoutingTable &table, RoutingTable::iterator record) {
  auto lower_bound = std::next(record);
  auto subtree_upper_address = record->first;
  subtree_upper_address.back() += 1;
  auto upper_bound = table.upper_bound(subtree_upper_address);
  return table.erase(lower_bound, upper_bound);
}

static Node *GetMostFrequentNeighbor(const std::vector<SarpRouting::RoutingTable::iterator> &children, Node const * reflexive_via_node) {
  std::map<Node *, int> counts;
  if (children.size() == 1) {
    return (*children.begin())->second.via_node;
  }
  for (const auto child : children) {
    // Don't propagate reflexive node
    if (child->second.via_node != reflexive_via_node) {
      ++counts[child->second.via_node];
    }
  }
  auto PairLessThan = [](std::pair<Node *, int> p1, std::pair<Node *, int> p2) {
                        return p1.second < p2.second;
                      };
  auto neighbor_maxcost_pair = std::max_element(counts.cbegin(), counts.cend(), PairLessThan);
  return neighbor_maxcost_pair->first;
}

std::pair<SarpRouting::CostWithNeighbor, bool> SarpRouting::GetGeneralized(RoutingTable &table, RoutingTable::iterator record, Node const * reflexive_via_node) {
  const auto children = GetDirectChildren(table, record);
  if (children.size() == 0) {
    return {CostWithNeighbor(), false};
  }
  if (children.size() == 1) {
    return {(*children.begin())->second, true};
  }
  std::vector<Cost> children_costs;
  for (const auto &child : children) {
    children_costs.push_back(child->second.cost);
  }
  return {{.cost = children_costs, .via_node = GetMostFrequentNeighbor(children, reflexive_via_node)}, true};
}

// Add record to the table if the cost (via Cost::PreferTo) is preffered among
// compared to these costs:
//   1) cost of an already existing record with the same address 
//   2) generalized cost of children of this node
//
// 2nd cost needs to be computed for children already present in the table
// therefore for correct functionality all children w.r.t. final state of the
// table have to be present at the time of addition of the parent i.e. record
// created by parameters.
//
// (*) Furthermore if the cost is prefered to the generalized cost of the
// children AddRecord deletes the whole subtree under the newly inserted record.
// That is to prevent further generalization of this node based on its children.
void SarpRouting::AddRecord(RoutingTable &table, 
    const Address &address, const Cost &cost, Node *via_neighbor, Node const * reflexive_via_node) {
  auto [matching_record, success] = table.insert({address, {cost, via_neighbor}});
  if (success) {
    // There is no record with given address.
    // Check (2)
    auto [generalized, has_generalized] = GetGeneralized(table, matching_record, reflexive_via_node);
    if (has_generalized && generalized.cost.PreferTo(cost)) {
      matching_record->second = generalized;
      // (*)
      RemoveSubtree(table, matching_record);
    }
  } else {
    // Since we are processing all updates in one batch a already inserted
    // record has to come from other neighbor.
    assert(matching_record->second.via_node != via_neighbor);
    // Check (1)
    if (cost.PreferTo(matching_record->second.cost)) {
      matching_record->second = {.cost = cost, .via_node = via_neighbor};
    }
  }
}

void SarpRouting::Generalize(RoutingTable &table, Node const * reflexive_via_node) {
  for (auto it = table.begin(); it != table.end(); ++it) {
    if (it->first.size() == 1) {
      GeneralizeRecursive(table, it, reflexive_via_node);
    }
  }
}

void SarpRouting::GeneralizeRecursive(RoutingTable &table, RoutingTable::iterator record, Node const * reflexive_via_node) {
  // Recursive call to generalize all children first.
  auto children = GetDirectChildren(table, record);
  if (children.size() == 0) {
    return;
  }
  if (children.size() == 1) {
    record->second = (*children.begin())->second;
    return;
  }
  for (auto &child : children) {
    GeneralizeRecursive(table, child, reflexive_via_node);
  }
  // Get costs of all direct children.
  std::vector<Cost> children_costs;
  for (auto &child : children) {
    children_costs.push_back(child->second.cost);
  }
  // Find a best via_node from the children the most frequent route which is not
  // reflective route if possible.
  record->second.via_node = GetMostFrequentNeighbor(children, reflexive_via_node);
  // Update this cost based on all direct children which are already dealt with.
  record->second.cost = Cost(children_costs);
}

void SarpRouting::Compact(RoutingTable &table, double treshold) {
  for (auto record = table.begin(); record != table.end();  /* no increment */) {
    if (IsRedundant(table, record, treshold)) {
      auto parent = GetParent(table, record);
      record = RemoveSubtree(table, parent);
    } else {
      ++record;
    }
  }
}

void SarpRouting::InsertInitialAddress(Address address, const Cost &cost) {
  while (address.size() > 0) {
    (void)working_.insert({address, {.cost = cost, .via_node = &node_}});
    address.pop_back();
  }
}

bool SarpRouting::BatchProcessUpdate(const Cost &neighbor_cost, const Cost &reflexive_cost, double treshold) {
  assert(neighbor_count_ == last_updates_.size());
  // Create an input batch from all incomming updates.
  std::multimap<Address, CostWithNeighbor> input_batch;
  for (const auto [neighbor, update_table] : last_updates_) {
    for (const auto [address, cost] : update_table) {
      Cost actual_cost = Cost::AddCosts(cost, neighbor_cost);
      input_batch.insert({address, {.cost = actual_cost, .via_node = neighbor}});
    }
  }
  // And add this node_ addresses.
  for (auto address : node_.get_addresses()) {
    while (address.size() > 0) {
      input_batch.insert({address, {.cost = reflexive_cost, .via_node = &node_}});
      address.pop_back();
    }
  }
  RoutingTable output;
  // First create table of shortest routes from all updates.
  // Process them in reverse order to have all children of any node already in
  // the table. That is the requirement for corret functioning of AddRecord
  // function.
  for (auto it = input_batch.rbegin(); it != input_batch.rend(); ++it) {
    auto &[address, cost_w_neighbor] = *it;
    SarpRouting::AddRecord(output, address, cost_w_neighbor.cost, cost_w_neighbor.via_node, &node_);
  }
  Generalize(output, &node_);
  Compact(output, treshold);
  bool change_occured = NeedUpdate(output, 0.9);
  working_ = output;
  return change_occured;
}

bool SarpRouting::NeedUpdate(const RoutingTable &new_table, double mean_difference_treshold) const {
  if (working_.size() != new_table.size()) {
    return true;
  }
  auto working_it = working_.cbegin();
  auto new_it = new_table.cbegin();
  while (working_it != working_.cend() && new_it != new_table.cend()) {
    if (working_it->first != new_it->first) {
      return true;
    } else {
      if (std::abs(working_it->second.cost.Mean() - new_it->second.cost.Mean())
          < mean_difference_treshold) {
        return true;
      }
    }
    ++working_it;
    ++new_it;
  }
  return false;
}

void SarpRouting::CreateUpdateMirror() {
  update_mirror_.clear();
  for (const auto &[address, cost_with_neighbor] : working_) {
    (void)update_mirror_.emplace(address, cost_with_neighbor.cost);
  }
  ++mirror_id_;
}

}  // namespace simulation
