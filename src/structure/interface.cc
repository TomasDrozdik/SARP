//
// interface.cc
//
#include <cassert>

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

Interface::~Interface() {
  if (is_valid_) {
    other_end_->is_valid_ = false;
  }
}

void Interface::Create(Node &node1, Node &node2) {
  if (&node1 == &node2) {
    auto i1 = node1.get_active_interfaces().
        insert(std::make_unique<Interface>(node1, node1));
    assert(i1.second);
    (*i1.first)->other_end_ = i1.first->get();
  } else {
    auto i1 = node1.get_active_interfaces().
        insert(std::make_unique<Interface>(node1, node2));
    assert(i1.second);
    (*i1.first)->other_end_ = i1.first->get();
    auto i2 = node2.get_active_interfaces().
        insert(std::make_unique<Interface>(node2, node1));
    assert(i2.second);
    // Now connect the two interfaces together
    (*i1.first)->other_end_ = i2.first->get();
    (*i2.first)->other_end_ = i1.first->get();
  }
}

Interface::Interface(Node &node, Node &other) :
    node_(&node), other_node_(&other), other_end_(nullptr) { }

void Interface::Send(std::unique_ptr<ProtocolPacket> packet) const {
  Simulation& simulation = Simulation::get_instance();
  // First check if the connection is still active
  if (!IsConnected()) {
    simulation.get_statistics().RegisterBrokenConnectionsSend();
    return;
  }
  Time delivery_duration =
      simulation.get_simulation_parameters().
          DeliveryDuration(*node_, *other_end_->node_, packet->get_size());
  simulation.ScheduleEvent(std::make_unique<RecvEvent>(delivery_duration, false,
      *other_end_, std::move(packet)));
}

void Interface::Recv(std::unique_ptr<ProtocolPacket> packet) {
  packet->UpdateTTL();
  node_->Recv(std::move(packet), this);
}

bool Interface::IsConnected() const {
  return is_valid_ && node_->get_connection().IsConnectedTo(*other_node_);
}

bool Interface::operator==(const Interface &other) const {
  return node_ == other.node_ && other_node_ == other.other_node_;
}

const Node &Interface::get_node() const {
  return *node_;
}

const Interface &Interface::get_other_end() const {
  return *other_end_;
}

const Node &Interface::get_other_end_node() const {
  return *other_node_;
}

}  // namespace simulation