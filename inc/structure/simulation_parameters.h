//
// simulation_parameters.h
//

#ifndef SARP_STRUCTURE_SIMULATION_PARAMETERS_H_
#define SARP_STRUCTURE_SIMULATION_PARAMETERS_H_

#include <cstddef>
#include <ostream>

#include "structure/node.h"

namespace simulation {

using Time = std::size_t;

class SimulationParameters {
  // TODO: create method to parse these parameters form outside file
  // TODO: create method to create event_generators from these parameters
  // TODO: figure out a way for these to be const
 public:
  // Static class.
  SimulationParameters() = delete;

  static bool IsMandatoryInitialized() {
    return duration_ && ttl_limit_ && connection_range_;
  }

  static std::ostream &Print(std::ostream &os);

  static Time DeliveryDuration(const Node &from, const Node &to,
                               const std::size_t packet_size);

  static void set_duration(Time time) { duration_ = time; }
  static Time get_duration() { return duration_; }

  static void set_ttl_limit(uint32_t limit) { ttl_limit_ = limit; }
  static uint32_t get_ttl_limit() { return ttl_limit_; }

  static void set_connection_range(uint32_t range) {
    connection_range_ = range;
  }
  static uint32_t get_connection_range() { return connection_range_; }

  static void set_traffic_start(Time time) { traffic_start_ = time; }
  static Time get_traffic_start() { return traffic_start_; }

  static void set_traffic_end(Time time) { traffic_end_ = time; }
  static Time get_traffic_end() { return traffic_end_; }

  static void set_traffic_event_count(std::size_t count) {
    traffic_event_count_ = count;
  }
  static Time get_traffic_event_count() { return traffic_event_count_; }

  static void set_reflexive_traffic(bool is_reflexive) {
    reflexive_traffic_ = is_reflexive;
  }
  static bool get_reflexive_traffic() { return reflexive_traffic_; }

 private:
  // Mandatory for
  static inline Time duration_ = 0;
  static inline uint32_t ttl_limit_ = 0;

  // Network creation.
  static inline uint32_t connection_range_ = 0;

  // TODO: 0 is reasonable so provide some default check
  static inline Time traffic_start_ = 0;
  static inline Time traffic_end_ = 0;
  static inline std::size_t traffic_event_count_ = 0;
  static inline bool reflexive_traffic_ = false;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_SIMULATION_PARAMETERS_H_
