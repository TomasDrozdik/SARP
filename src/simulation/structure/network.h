//
// network.h
//

#ifndef SARP_SIMULATION_STRUCTURE_NETWORK_H_
#define SARP_SIMULATION_STRUCTURE_NETWORK_H_

#include <memory>
#include <vector>

#include "simulation.h"
#include "statistics.h"
#include "node.h"

namespace simulation {

using Time = std::size_t;
class Event;

class AbstractNetworkFactory {
 public:
  virtual std::unique_ptr<Network> Create() const = 0;
};

class SimulationParameters {
 public:
  SimulationParameters(Time simulation_duration, int signal_speed_Mbps);
  ~SimulationParameters() = default;

  Time DeliveryDuration(const Node &from, const Node &to,
      const size_t packet_size) const;

  const Time simulation_duration;
  const int signal_speed_Mbps;
};

class Network {
 friend class Simulation;
 public:
  Network(std::unique_ptr<std::vector<Node>> nodes,
      std::unique_ptr<std::vector<std::unique_ptr<Event>>> events,
      std::unique_ptr<SimulationParameters> simulation_parameters);
  ~Network() = default;

 private:
  std::unique_ptr<std::vector<Node>> nodes_;
  std::unique_ptr<std::vector<std::unique_ptr<Event>>> events_;
  std::unique_ptr<SimulationParameters> simulation_parameters_;
  std::unique_ptr<Statistics> statistics_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_NETWORK_H_
