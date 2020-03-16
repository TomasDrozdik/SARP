//
// routing_table.h
//

#ifndef SARP_SARP_ROUTING_TABLE_H_
#define SARP_SARP_ROUTING_TABLE_H_

#include <unordered_map>
#include <vector>

#include "sarp/address.h"
#include "structure/interface.h"

namespace simulation {

class SarpRoutingTable {
 public:
  struct Record {
    Record() = default;
    Record(Interface *via_interface, double cost_mean,
           double cost_standard_deviation, std::size_t group_size);

    void MergeWith(const Record &other);

    // Add more interfaces
    Interface *via_interface = nullptr;  // if this is null Record is invalid
    double cost_mean = 0.0;
    double cost_standard_deviation = 0.0;
    double group_size = 0.0;  // In log scale with base 1.1
  };

  // Initialize with active interfaces on given Node
  void Init(const Node &node);

  // Go through all interfaces in Records and invalidate unconnected.
  void UpdateInterfaces(const Node &node);

  bool Merge(const SarpRoutingTable &update,
             const Interface *processing_interface);

  Interface *FindBestMatch(const SarpAddress &addr);

  SarpRoutingTable CreateAggregate() const;

  uint32_t get_size() const;

 private:
  struct TreeNode {
    std::unordered_map<SarpAddress::AddressComponentType,
                       std::unique_ptr<TreeNode>>
        children;
    Record record;
  };

  // Recursive implemantation of UpdateInterfaces()
  void UpdateInterfacesImpl(TreeNode &base_node);

  TreeNode root_;
};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_TABLE_H_
