//
// network.cc
//

#include "network.h"

namespace simulation {

SimulationParameters::SimulationParameters(Time simulation_duration,
    int signal_speed_Mbps) :
        simulation_duration(simulation_duration),
        signal_speed_Mbps(signal_speed_Mbps) { }

Time SimulationParameters::DeliveryDuration(const Node &from, const Node &to,
    const std::size_t packet_size_bytes) const {
  // This calculation is purely based on averagy wifi speed in Mbps and
  // packet size.
  // TODO: form nodes we can obtain also their distance via Position::Distance
  //       this could be used for more precise calculation.
  // Returns time in micro seconds
  return (packet_size_bytes * 8) / signal_speed_Mbps;
}

Network::Network(std::unique_ptr<std::vector<std::unique_ptr<Node>>> nodes,
    std::unique_ptr<std::vector<std::unique_ptr<Event>>> events,
    std::unique_ptr<SimulationParameters> simulation_parameters) :
        nodes_(std::move(nodes)), events_(std::move(events)),
        simulation_parameters_(std::move(simulation_parameters)) {
  statistics_ = std::make_unique<Statistics>(*this);
  for (std::size_t i = 0; i < nodes_->size(); ++i) {
    nodes_->operator[](i)->UpdateConnections(*nodes_);
  }
}

}  // namespace simulation