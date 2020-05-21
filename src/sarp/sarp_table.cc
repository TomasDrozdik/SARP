//
// sarp_table.cc
//

#include "sarp/sarp_table.h"

#include <cassert>

namespace simulation {

void SarpTable::AddRecord(const Address &address, const Cost &cost, Node *via_neighbor, Node const *reflexive_via_node) {
  auto [matching_record, success] = data_.insert({address, {cost, via_neighbor}});
  if (!success) {
    if (cost.PreferTo(matching_record->second.cost)) {
      matching_record->second = {.cost = cost, .via_node = via_neighbor};
    }
  }
}

void SarpTable::Generalize(Node const *reflexive_via_node) {
  for (auto it = data_.begin(); it != data_.end(); ++it) {
    if (it->first.size() == 1) {
      GeneralizeRecursive(it, reflexive_via_node);
    }
  }
}

void SarpTable::Compact(double compact_treshold, double min_standard_deviation) {
  for (auto record = data_.begin(); record != data_.end();  /* no increment */) {
    if (HasRedundantChildren(record, compact_treshold, min_standard_deviation)) {
      record = RemoveSubtree(record);
    } else {
      ++record;
    }
  }
}

bool SarpTable::NeedUpdate(const SarpTable &new_table, double mean_difference_treshold) const {
#if 1
  for (auto update_record = new_table.cbegin(); update_record != new_table.cend(); ++update_record) {
    auto matching_record = Find(update_record->first);
    if (std::abs(matching_record->second.cost.Mean() - update_record->second.cost.Mean())
          > mean_difference_treshold) {
      return true;
    }
  }
  return false;
# else 
  if (Size() != new_table.Size()) {
    return true;
  }
  auto this_it = data_.cbegin();
  auto new_it = new_table.cbegin();
  while (this_it != data_.cend() && new_it != new_table.cend()) {
    if (this_it->first != new_it->first) {
      return true;
    } else {
      if (std::abs(this_it->second.cost.Mean() - new_it->second.cost.Mean())
          > mean_difference_treshold) {
        return true;
      }
    }
    ++this_it;
    ++new_it;
  }
  return false;
#endif
}

SarpUpdate SarpTable::CreateUpdate() const {
  SarpUpdate result;
  for (const auto &[address, cost_with_neighbor] : data_) {
    (void)result.emplace(address, cost_with_neighbor.cost);
  }
  return result;
}

std::vector<SarpTable::SarpTable::iterator> SarpTable::GetDirectChildren(iterator parent) {
  assert(parent->first.size() > 0);
  auto lower_bound = parent;
  Address upper_address_bound = parent->first;
  upper_address_bound.back() += 1;  // Increase the last address component.
  auto upper_bound = data_.upper_bound(upper_address_bound);
  // Now search for records with child address length in the lower - upper bound
  // range.
  auto direct_child_address_size = parent->first.size() + 1;
  std::vector<iterator> children;
  for (auto possible_child = lower_bound; possible_child != upper_bound; ++possible_child) {
    if (possible_child->first.size() == direct_child_address_size) {
      children.push_back(possible_child);
    }
  }
  return children;
}

bool SarpTable::HasRedundantChildren(iterator record, double compact_treshold, double min_standard_deviation) {
  assert(record != data_.end());
  auto mean = record->second.cost.Mean();
  auto sd = record->second.cost.StandardDeviation();
  if (sd < min_standard_deviation) {
    sd = min_standard_deviation;
    auto variance = sd * sd;
    record->second.cost = Cost{mean, variance};
  }
  return mean/sd > compact_treshold;
}

SarpTable::iterator SarpTable::RemoveSubtree(iterator record) {
  auto lower_bound = std::next(record);
  auto subtree_upper_address = record->first;
  subtree_upper_address.back() += 1;
  auto upper_bound = data_.lower_bound(subtree_upper_address);
  return data_.erase(lower_bound, upper_bound);
}

Node *SarpTable::GetMostFrequentNeighbor(const std::vector<iterator> &children, Node const *reflexive_via_node) {
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

void SarpTable::GeneralizeRecursive(iterator record, Node const *reflexive_via_node) {
  // Recursive call to generalize all children first.
  auto children = GetDirectChildren(record);
  if (children.size() == 0) {
    return;
  }
  for (auto &child : children) {
    GeneralizeRecursive(child, reflexive_via_node);
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

}  // namespace simulation
