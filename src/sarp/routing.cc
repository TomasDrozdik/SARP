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
  auto upper_bound = table.lower_bound(subtree_upper_address);
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

// Add record to the table if the cost (via Cost::PreferTo) is preffered among
// compared to cost of an already existing record with the same address if
// present.
void SarpRouting::AddRecord(RoutingTable &table, 
    const Address &address, const Cost &cost, Node *via_neighbor, Node const * reflexive_via_node) {
  auto [matching_record, success] = table.insert({address, {cost, via_neighbor}});
  if (!success) {
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
  // Now pick prefered record among already existing one and generalized one.
  Cost generalized_cost(children_costs);
  if (generalized_cost.PreferTo(record->second.cost)) {
    record->second.cost = generalized_cost;
    // Find a best via_node from the children the most frequent route which is not
    // reflective route if possible.
    record->second.via_node = GetMostFrequentNeighbor(children, reflexive_via_node);
  }
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
  auto &inputs = last_updates_;
  RoutingTable output;
  // Insert local routs to input.
  for (auto address : node_.get_addresses()) {
    SarpRouting::AddRecord(output, address, reflexive_cost, &node_, &node_);
    address.pop_back();
    Cost max_cost(1000, 1000);
    while (address.size() > 0) {
      SarpRouting::AddRecord(output, address, max_cost, &node_, &node_);
      address.pop_back();
    }
  }
  // Create table of shortest routes from all updates.
  for (const auto &[via_node, update_table] : inputs) {
    for (const auto &[address, cost] : update_table) {
      Cost actual_cost = Cost::AddCosts(cost, neighbor_cost);
      SarpRouting::AddRecord(output, address, actual_cost, via_node, &node_);
    }
  }
  Generalize(output, &node_);
  Compact(output, treshold);
  bool change_occured = NeedUpdate(output, 0.9);
  working_ = output;
  last_updates_.clear();
  Dump(std::cerr);
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
