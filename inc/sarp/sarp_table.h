//
// sarp_table.h
//

#ifndef SARP_SARP_TABLE_H_
#define SARP_SARP_TABLE_H_

#include <map>
#include <vector>

#include "sarp/cost.h"
#include "structure/node.h"
#include "structure/types.h"

namespace simulation {

using SarpUpdate = std::map<Address, Cost>;

class SarpTable final {
 public:
  struct CostWithNeighbor {
    Cost cost;
    Node *via_node;
  };

  using Data = std::map<Address, CostWithNeighbor>;
  using iterator = Data::iterator;
  using const_iterator = Data::const_iterator;

  const_iterator cbegin() const { return data_.cbegin(); }

  const_iterator cend() const { return data_.cend(); }

  iterator begin() { return data_.begin(); }

  iterator end() { return data_.end(); }

  bool Contains(const Address &address) const {
    return data_.contains(address);
  }

  std::pair<iterator, bool> Insert(const Address &address, const Cost &cost,
                                   Node *via_node) {
    // TODO emplace
    return data_.insert({address, {.cost = cost, .via_node = via_node}});
  }

  iterator Erase(iterator record) { return data_.erase(record); }

  iterator Find(const Address &address) { return data_.find(address); }

  const_iterator Find(const Address &address) const {
    return data_.find(address);
  }

  std::size_t Size() const { return data_.size(); }

  void AddRecord(const Address &address, const Cost &cost, Node *via_neighbor,
                 Node const *reflexive_via_node);

  void Generalize(Node const *reflexive_via_node);

  void Compact(double compact_treshold, double min_standard_deviation);

  bool NeedUpdate(const SarpTable &new_table, double difference_treshold,
                  double ratio_variance_treshold) const;

  SarpUpdate CreateUpdate() const;

  std::pair<Address, bool> FindFreeSubtreeAddress(
      const_iterator root, range<AddressComponent> component_range) const;

 private:
  std::vector<iterator> GetDirectChildren(iterator record);

  bool HasRedundantChildren(iterator record, double compact_treshold,
                            double min_standard_deviation);

  iterator RemoveSubtree(iterator record);

  Node *GetMostFrequentNeighbor(const std::vector<iterator> &children,
                                Node const *reflexive_via_node);

  void GeneralizeRecursive(iterator record, Node const *reflexive_via_node);

  Data data_;
};

}  // namespace simulation

#endif  // SARP_SARP_TABLE_H_
