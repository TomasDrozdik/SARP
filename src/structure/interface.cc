//
// interface.cc
//

#include "interface.h"

#include <deque>
#include <iostream>
#include <memory>

#include "simulation.h"
#include "event.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Interface &iface) {
  return os << "iface_on:" << iface.get_node() << ":[" << &iface <<
      "]:other_end" << iface.get_other_end_node() << ":[" <<
      &iface.get_other_end() << ']';
}

void Interface::Create(Node &node1, Node &node2) {
  if (&node1 == &node2) {
    node1.get_active_connections().
        push_back(std::make_unique<Interface>(node1));
    node1.get_active_connections().back()->other_end_ =
        node1.get_active_connections().back().get();
  } else {
    node1.get_active_connections().push_back(
        std::make_unique<Interface>(node1));
    node2.get_active_connections().push_back(
        std::make_unique<Interface>(node2));
    // Now connect the two interfaces together
    node1.get_active_connections().back()->other_end_ =
        node2.get_active_connections().back().get();
    node2.get_active_connections().back()->other_end_ =
        node1.get_active_connections().back().get();
  }
}

Interface::Interface(Node &node) : node_(node), other_end_(nullptr) { }

void Interface::Send(std::unique_ptr<ProtocolPacket> packet) const {
  Simulation& simulation = Simulation::get_instance();
  Time delivery_duration =
      simulation.get_simulation_parameters().
          DeliveryDuration(node_, other_end_->node_, packet->get_size());
  simulation.ScheduleEvent(std::make_unique<RecvEvent>(delivery_duration,
      *other_end_, std::move(packet)));
}

void Interface::Recv(std::unique_ptr<ProtocolPacket> packet) {
  // TODO: this may be neccesary node_.Recv(std::move(packet), *this);
  node_.Recv(std::move(packet));
}

const Node &Interface::get_node() const {
  return node_;
}

const Interface &Interface::get_other_end() const {
  return *other_end_;
}

const Node &Interface::get_other_end_node() const {
  return other_end_->node_;
}

}  // namespace simulation
