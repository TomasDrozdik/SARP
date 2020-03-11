//
// routing.cc
//

#include "sarp/routing.h"

#include "sarp/update_packet.h"

namespace simulation {

SarpRouting::SarpRouting(Node &node) : Routing(node) { }

Interface *SarpRouting::Route(ProtocolPacket &packet) {
  return table_.FindBestMatch(dynamic_cast<const SarpAddress &>(
      *packet.get_destination_address()));
}

bool SarpRouting::Process(ProtocolPacket &packet,
    Interface *processing_interface) {
  Simulation::get_instance().get_statistics().
      RegisterRoutingOverheadDelivered();

  auto &update_packet = dynamic_cast<SarpRoutingUpdate &>(packet);
  bool change_occured = UpdateRouting(update_packet.update,
      processing_interface);

  Time current_time = Simulation::get_instance().get_current_time();
  if (change_occured) {
    Time due_update = current_time - last_update_;
    if (due_update > update_period_) {
      last_update_ = current_time;
      // Do an instantanious Update() without UpdateEvent.
      Update();
    } else {
      // Plan the update at the given period.
      Time time_to_period = current_time % update_period_;
      last_update_ = current_time + time_to_period;
      Simulation::get_instance().ScheduleEvent(
          std::make_unique<UpdateRoutingEvent>(last_update_, true, *this));
    }
  }
  return false;
}

void SarpRouting::Init() {
  table_.Init(node_);
}

void SarpRouting::Update() {
  for (auto &interface : node_.get_active_interfaces()) {
    // Skip over reflexive interfaces.
    if (&interface->get_other_end_node() == &node_) {
      continue;
    }
    // Create update packet.
    std::unique_ptr<ProtocolPacket> packet =
        std::make_unique<SarpRoutingUpdate>(
          node_.get_address()->Clone(),
          interface->get_other_end_node().get_address()->Clone(),
          table_.CreateAggregate());
    // Add to statistics before we move unique_ptr<Packet>
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSend();
    Simulation::get_instance().get_statistics().RegisterRoutingOverheadSize(
        packet->get_size());
    // Schedule immediate send.
    Simulation::get_instance().ScheduleEvent(std::make_unique<SendEvent>(
        1, false, node_, std::move(packet)));
  }
}

void SarpRouting::UpdateInterfaces() {
  // Invalidate not connected Interfaces.
  table_.UpdateInterfaces(node_);
  // Now delete not connected interfaces from node_.
  for (auto it = node_.get_active_interfaces().begin();
      it != node_.get_active_interfaces().end(); ) {
    if (!(*it)->IsConnected()) {
      it = node_.get_active_interfaces().erase(it);
    } else {
      ++it;
    }
  }
}

bool SarpRouting::UpdateRouting(const SarpRoutingTable &update,
    Interface *processing_interface) {
  return table_.Merge(update, processing_interface);
}

}  // namespace simulation
