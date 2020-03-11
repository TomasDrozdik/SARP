//
// network.cc
//

#include "structure/network.h"

#include <algorithm>
#include <iostream>

#include "structure/simulation.h"

namespace simulation {

Network::Network(std::vector<std::unique_ptr<Node>> nodes) :
    nodes_(std::move(nodes)) {
  // Initialize the network connections.
  UpdateInterfaces();
  // Initialize network routing.
  for (auto &node : nodes_) {
    // ALERT: Virtual method call in ctor. It it better than doing an Init()
    // because then initializing checks would be necessary.
    node->get_routing().Init();
  }
  for (auto &node : nodes_) {
    Simulation::get_instance().ScheduleEvent(
        std::make_unique<UpdateRoutingEvent>(0, true, node->get_routing()));
  }
}

void Network::UpdateInterfaces() {
  if (nodes_.size() < 2) {
    return;
  }
  // Create template Interface for finding existing ones.
  auto key = std::make_unique<Interface>(*nodes_[0], *nodes_[1]);

  // Now go through every distinct pair of nodes and if they both see each other
  // via Connection::IsConnectedTo then Create Interface between them.
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    for (std::size_t j = i; j < nodes_.size(); ++j) {
      if (nodes_[i]->get_connection().IsConnectedTo(*nodes_[j]) &&
          nodes_[j]->get_connection().IsConnectedTo(*nodes_[i])) {
        // Find if such interface exists.
        key->node_ = nodes_[i].get();
        key->other_node_ = nodes_[j].get();
        auto search = nodes_[i]->get_active_interfaces().find(key);
        if (search == nodes_[i]->get_active_interfaces().end() ||
            !(*search)->is_valid_) {
          // If it doesn't exit or there is an existing one with already
          // destroyed other side then create a new one.
          Interface::Create(*nodes_[i], *nodes_[j]);
        }
      }
    }
  }
}

void Network::ExportToDot(std::ostream &os) const {
  // Mark this as strict graph to remove duplicate edges.
  os << "strict graph G {\n";
  // Assign position to all nodes.
  for (auto &node : nodes_) {
    os << '\t' << *node->get_address() << " [ " <<
        node->get_connection().position << " ]\n";
  }
  // Print all the edges. Go through all interfaces.
  for (auto &node : nodes_) {
    for (auto &iface : node->get_active_interfaces()) {
      if (node.get() != &iface->get_other_end_node()) {
        os << '\t' << *node->get_address() << " -- " <<
            *iface->get_other_end_node().get_address() << '\n';
      }
    }
  }
  os << "}\n";
}

}  // namespace simulation
