//
// parameters_builder.h
//

#ifndef SARP_STRUCTURE_PARAMETERS_BUILDER_H_
#define SARP_STRUCTURE_PARAMETERS_BUILDER_H_

#include <cstddef>
#include <ostream>

#include "network_generator/position_generator.h"
#include "structure/node.h"
#include "structure/position.h"
#include "structure/simulation.h"

namespace simulation {

using Time = std::size_t;

class SimulationParametersBuilder {
 public:
  SimulationParametersBuilder(
      RoutingType routing_type, uint32_t node_count, Time duration,
      uint32_t ttl_limit, uint32_t connection_range, Position position_min,
      Position position_max,
      std::unique_ptr<PositionGenerator> initial_positions);

  SimulationParametersBuilder &AddTraffic(Time start, Time end,
                                          std::size_t event_count);

  SimulationParametersBuilder &AddRandomMovement(Time start, Time end,
                                                 Time step_period,
                                                 double speed_min,
                                                 double speed_max,
                                                 Time pause_min, Time pause_max,
                                                 Time neighbor_update_period);

  SimulationParametersBuilder &AddPeriodicRoutingUpdate(Time update_period);

  SimulationParameters Build();

 private:
  // General
  RoutingType routing_type_;
  uint32_t node_count_;
  Time duration_;
  uint32_t ttl_limit_;
  uint32_t connection_range_;
  Position position_min_;
  Position position_max_;
  std::unique_ptr<PositionGenerator> initial_positions_;

  // Traffic generation parameters.
  bool has_traffic_ = false;
  Time traffic_start_ = 0;
  Time traffic_end_ = 0;
  std::size_t traffic_event_count_ = 0;

  // Movement simulation parameters.
  bool has_movement_ = false;
  Time move_start_ = 0;
  Time move_end_ = 0;
  Time move_step_period_ = 0;
  double move_speed_min_ = 0;  // m/s
  double move_speed_max_ = 0;  // m/s
  Time move_pause_min_ = 0;
  Time move_pause_max_ = 0;
  Time neighbor_update_period_ = 0;
  std::unique_ptr<PositionGenerator> move_directions_ = 0;

  // Periodic routing update parameters.
  bool has_periodic_routing_update_ = false;
  Time routing_update_period_ = 0;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_PARAMETERS_BUILDER_H_
