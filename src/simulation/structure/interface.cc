//
// interface.cc
//

#include "interface.h"

#include <deque>

#include "simulation.h"
#include "event.h"

namespace simulation {

void Interface::Create(Node &node1, Node &node2) {
  node1.get_active_connections().push_back(Interface(node2));
  Interface &i1 = node1.get_active_connections().back();
  node2.get_active_connections().push_back(Interface(node1));
  Interface &i2 = node2.get_active_connections().back();
  i1.other_end_ = &i2;
  i2.other_end_ = &i1;
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
  node_.Recv(std::move(packet), *this);
}

void Interface::Print() {
  node_.Print();
}

const Interface &Interface::get_other_end() const {
  return *other_end_;
}

const Node &Interface::get_other_end_node() const {
  return other_end_->node_;
}

}  // namespace simulation
