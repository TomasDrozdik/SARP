//
// interface.cc
//

#include "interface.h"

#include <deque>

#include "simulation.h"
#include "event.h"

namespace simulation {

Interface::Interface(const Node *node, Node *other_end) :
    node_(node), other_end_(other_end) { }

void Interface::Send(std::unique_ptr<ProtocolPacket> packet) const {
  Simulation& simulation = Simulation::get_instance();
  Time delivery_duration =
      simulation.get_simulation_parameters().
      DeliveryDuration(*node_, *other_end_, packet->get_size());
  simulation.ScheduleEvent(std::make_unique<RecvEvent>(delivery_duration,
      *other_end_, std::move(packet)));
}

const Node* Interface::get_other_end() const {
  return other_end_;
}

}  // namespace simulation
