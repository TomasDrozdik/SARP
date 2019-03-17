//
// interface.cc
//

#include "interface.h"

#include <deque>

#include "scheduler.h"
#include "tasks.h"

namespace simulation {

Interface::Interface(Node &node, Node &other_end) :
    node_(node), other_end_(other_end) {}

void Interface::Send(std::unique_ptr<ProtocolPacket> &&packet) const {
  Time delivery_duration = Timings::DeliveryDuration(node_, other_end_,
      packet->get_size());
  new RecvTask(delivery_duration, other_end_, std::move(packet));

  // Calculate the time of processing cycle on reviever node and schedule
  // process task.
  // +1 is because of the fact that schedulers std::priority_queue does not
  // guarantee any ordering on same priority objects. Therefore by +1 we prevent
  // ProcessTask happening before RecvTask.
  Time node_processing_cycle = (delivery_duration + 1 +
      Scheduler::get_instance().get_current_time()) %
      node_.get_processing_power();
  new ProcessTask(delivery_duration + 1 + node_processing_cycle, other_end_);
}

const Address& Interface::get_other_end_addr() const {
  return other_end_.get_address();
}

}  // namespace simulation
