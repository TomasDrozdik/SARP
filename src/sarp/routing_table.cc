//
// routing_table.cc
//

#include "routing_table.h"

#include <cmath>

namespace simulation {

SarpRoutingTable::Record::Record(Interface *via_interface, double cost_mean,
    double cost_standard_deviation, std::size_t group_size) :
        via_interface(via_interface), cost_mean(cost_mean),
        cost_standard_deviation(cost_standard_deviation),
        group_size(group_size) { }

void SarpRoutingTable::Record::MergeWith(const Record &other) {
  // TODO:
}

void SarpRoutingTable::Init(const Node &node) {
  for (const auto &iface : node.get_active_interfaces()) {
    auto addr = dynamic_cast<SarpAddress &>(
        *iface->get_other_end_node().get_address());
    TreeNode *node = &root;
    for (auto address_component : addr.get_address()) {
      // Add new element, if one is already present move to next one.
      node = node->children.emplace(
          address_component, std::make_unique<TreeNode>()).first->second.get();
    }
    // Set proper values for given neighbor.
    node->record.via_interface = iface.get();
    node->record.cost_mean = 1;
    node->record.cost_standard_deviation = 1;
    node->record.group_size = 0;
  }
}

void SarpRoutingTable::UpdateInterfaces(const Node &node) {
  CheckInterfaces(root);
  Init(node);
}

bool SarpRoutingTable::Merge(const SarpRoutingTable &update,
    const Interface *processing_interface) {
  return false;
}

Interface *SarpRoutingTable::FindBestMatch(
    const SarpAddress &addr) {
  TreeNode const *node = &root;
  auto &addr_data = addr.get_address();
  std::size_t i = 0;
  auto found_node = node->children.find(addr_data[i++]);
  while (found_node != node->children.end()) {
    node = found_node->second.get();
    found_node = node->children.find(addr_data[i++]);
  }
  // We have hit maximum common prefix with the given address.
  // If the record does not exist check for children records and pick the best
  // one.
  if (node->record.via_interface) {
    return node->record.via_interface;
  } else {
    // TODO: find best route from the children
    return nullptr;
  }
}

SarpRoutingTable SarpRoutingTable::CreateAggregate() const {
  return SarpRoutingTable();
}

uint32_t SarpRoutingTable::get_size() const {
  // TODO:
  return 0;
}

// Go recursively through all interfaces and when the interface in Record
// istn't connected reset it to nullptr;
void SarpRoutingTable::CheckInterfaces(TreeNode &base_node) {
  if (base_node.record.via_interface->IsConnected()) {
    base_node.record.via_interface = nullptr;
  }
  for (auto &it : base_node.children) {
    CheckInterfaces(*it.second);
  }
}

}  // namespace simulation
