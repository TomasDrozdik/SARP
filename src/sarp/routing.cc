//
// routing.cc
//

#include "sarp/routing.h"

#include <cassert>
#include <cmath>

#include "sarp/update_packet.h"
#include "structure/types.h"
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

// TODO
Node *SarpRouting::Route(Env &env, Packet &packet) {
  const Address &destination_address = packet.get_destination_address();

  auto i = table_.Find(destination_address);
  if (i != table_.end()) {
    return i->second.via_node;
  }

  // Find longest common prefix addresses in forwarding table.
  auto it = table_.cbegin();
  if (it == table_.cend()) {
    return nullptr;
  }
  std::size_t lcp = 0;
  auto best_match = it->second;
  while (it != table_.cend()) {
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
    env.stats.RegisterReflexiveRoutingResult();
    return nullptr;  // There is no reflexive traffic.
  }
  return best_match.via_node;
}

void SarpRouting::Process(Env &env, Packet &packet, Node *from_node) {
  assert(packet.IsRoutingUpdate());
  env.stats.RegisterRoutingOverheadDelivered();
  auto &update_packet = dynamic_cast<SarpUpdatePacket &>(packet);
  last_updates_[from_node] = update_packet.RetrieveUpdate();
  if (last_updates_.size() == neighbor_count_) {
    change_occured_ = BatchProcessUpdate(env.parameters.get_sarp_parameters());
    if (change_occured_) {
      CreateUpdateMirror();
      NotifyChange();
    }
  } 
}

void SarpRouting::Init(Env &env) {
  for (const auto &address : node_.get_addresses()) {
    InsertInitialAddress(address, MIN_COST); 
  }
  CreateUpdateMirror();
  CheckPeriodicUpdate(env);
}

void SarpRouting::SendUpdate(Env &env, Node *neighbor) {
  assert(node_.get_neighbors().contains(neighbor));
  assert(neighbor != &node_);
  // Create update packet.
  std::unique_ptr<Packet> packet = std::make_unique<SarpUpdatePacket>(
      node_.get_address(), neighbor->get_address(), update_mirror_);
  // Register to statistics before we move packet away.
  env.stats.RegisterRoutingOverheadSend();
  env.stats.RegisterRoutingOverheadSize(packet->get_size());
  // Schedule immediate recieve on neighbor to bypass Node::Send() which calls
  // Routing::Route which is not desired.
  env.simulation.ScheduleEvent(std::make_unique<RecvEvent>(
      1, TimeType::RELATIVE, node_, *neighbor, std::move(packet)));
}

void SarpRouting::UpdateNeighbors(Env &env, const std::set<Node *> &current_neighbors) {
  // Search for invalid records in routing table.
  for (auto it = table_.begin(); it != table_.end();
       /* no increment */) {
    Node *neighbor = it->second.via_node;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_general().connection_range)) {
      assert(current_neighbors.contains(neighbor));
      ++it;
    } else {
      assert(!current_neighbors.contains(neighbor));
      it = table_.Erase(it);
    }
  }
  // Now clear the update history of invalid records.
  for (auto it = last_updates_.cbegin(); it != last_updates_.cend(); /* no increment */) {
    Node *neighbor = it->first;
    if (node_.IsConnectedTo(*neighbor, env.parameters.get_general().connection_range)) {
      assert(current_neighbors.contains(neighbor));
      ++it;
    } else {
      assert(!current_neighbors.contains(neighbor));
      it = last_updates_.erase(it);
    }
  }
  // Set the neighbor count to know the new batch size.
  neighbor_count_ = current_neighbors.size() - 1;  // -1 for reflexive node
  // If there are new neighbors set change_occured for next CheckPeriodicUpdate.
  const auto &old_neighbors = node_.get_neighbors();
  for (const auto current_neighbor : current_neighbors) {
    if (old_neighbors.contains(current_neighbor) == false) {
      change_occured_ = true;
      break;
    }
  }
}

void SarpRouting::UpdateAddresses() {
  change_occured_ = true;
}

static Address LCP(const std::set<Address> &set) {
  assert(!set.empty());
  if (set.size() == 1) {
    return *set.begin();
  }
  auto &first = *set.begin();
  auto &last = *set.rbegin();
  auto max_prefix = std::min(first.size(), last.size());

  std::size_t lcp;
  for (lcp = 0; lcp < max_prefix && first[lcp] == last[lcp]; ++lcp);
  return Address(first.cbegin(), first.cbegin() + lcp);
}

static Address PickNewAddress(const std::set<Address> &neighbor_addresses,
    AddressComponent min) {
  // TODO: pick a random one
  Address new_address = *neighbor_addresses.begin();
  new_address.push_back(min);
  return new_address;
}

std::pair<Address, bool> SarpRouting::SelectAddress(Env &env) const {
#ifdef DEBUG
  std::cerr << "AddressSelectionProcedure for " << node_ << '\n';
  std::cerr << "Neighbor addresses: ";
#endif
  // Find the parent node of all direct neighbor records.
  std::set<Address> neighbor_addresses;  // Sorted.
  for (auto it = table_.cbegin(); it != table_.cend(); ++it) {
    if (it->second.cost.Mean() == env.parameters.get_sarp_parameters().neighbor_cost.Mean()) {
      auto [inserted_address, success] = neighbor_addresses.insert(it->first);
      assert(success);
#ifdef DEBUG
      std::cerr << *inserted_address << ' ';
#endif
    }
  }
#ifdef DEBUG
      std::cerr << '\n';
#endif
  // If there are no neighbors pick node id.
  if (neighbor_addresses.empty()) {
    // Perform narrowing on the Node::ID to AddressComponent.
#ifdef DEBUG
      std::cerr << "Pick default address: " << node_.get_id() << '\n';
#endif
    return {Address({static_cast<AddressComponent>(node_.get_id())}), true};
  }
  // Find LCP of all neighbor records.
  Address lcp_address = LCP(neighbor_addresses);
  if (lcp_address.size() == 0) {
    auto new_address = PickNewAddress(neighbor_addresses, 0);
    assert(table_.Contains(new_address) == false);
#ifdef DEBUG
    std::cerr << "LCP == 0 prolong one neighbor: " << new_address << '\n';
#endif
    return {new_address, true};
  }
  // LCP address should be present since all records have generalized versions
  // of them.
  const auto common_neighbor_parent = table_.Find(lcp_address);
  assert(common_neighbor_parent != table_.cend());
  const auto [free_address, found] = table_.FindFreeSubtreeAddress(
      common_neighbor_parent, {0, 7});  // TODO add parameter
  if (found) {
    assert(table_.Contains(free_address) == false);
#ifdef DEBUG
  std::cerr << "Picking free address " << free_address << '\n';
#endif
    return {free_address, true};
  }
  // If there is no free address among subtree of all neighbors pick the first
  // neighbor and pick a longer address from it.
  auto new_address = PickNewAddress(neighbor_addresses, 0);
  assert(table_.Contains(new_address) == false);
#ifdef DEBUG
  std::cerr << "No freee address - prolong one neighbor " << new_address << '\n';
#endif
  return {new_address, true};
}

void SarpRouting::Dump(std::ostream &os) const {
  os << "Routing table dump: " << node_ << '\n'
     << "address,\t"
     << "cost,\t\t\t\t\t"
     << "via_node, "
     << "generalize\n";
  for (auto record = table_.cbegin(); record != table_.cend(); ++record) {
    os << record->first << "\t\t"
       << record->second.cost << "\t\t"
       << *record->second.via_node << "\n";
  }
}

void SarpRouting::InsertInitialAddress(Address address, const Cost &cost) {
  while (address.size() > 0) {
    (void)table_.Insert(address, cost, &node_);
    address.pop_back();
  }
}

bool SarpRouting::BatchProcessUpdate(const Parameters::Sarp &parameters) {
  assert(neighbor_count_ == last_updates_.size());
  auto &inputs = last_updates_;
  SarpTable output;
  // Insert local routs to input.
  for (auto address : node_.get_addresses()) {
    output.AddRecord(address, MIN_COST, &node_, &node_);
    address.pop_back();
    while (address.size() > 0) {
      output.AddRecord(address, MAX_COST, &node_, &node_);
      address.pop_back();
    }
  }
  // Create table of shortest routes from all updates.
  for (const auto &[via_node, update_table] : inputs) {
    for (const auto &[address, cost] : update_table) {
      Cost actual_cost = Cost::AddCosts(cost, parameters.neighbor_cost);
      output.AddRecord(address, actual_cost, via_node, &node_);
    }
  }
  output.Generalize(&node_);
  output.Compact(parameters.compact_treshold, parameters.min_standard_deviation);
  bool change_occured = table_.NeedUpdate(output, parameters.update_treshold);
  table_ = output;
  last_updates_.clear();
  return change_occured;
}

void SarpRouting::CreateUpdateMirror() {
  update_mirror_ = table_.CreateUpdate();
}

}  // namespace simulation
