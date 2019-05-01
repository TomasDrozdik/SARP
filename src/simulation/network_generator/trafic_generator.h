//
// trafic_generator.h
//

#ifndef SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFIC_GENERATOR_H_
#define SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFIC_GENERATOR_H_

#include "../structure/event.h"

#include <vector>
#include <memory>

#include "../structure/network.h"
#include "../structure/node.h"

namespace simulation {

class TraficGenerator : public EventGenerator {
 public:
  TraficGenerator(const std::vector<std::unique_ptr<Node>> &nodes,
      Time start, Time end);

  std::unique_ptr<Event> operator++() override;
 private:
  const std::vector<std::unique_ptr<Node>> &nodes_;
  Time start_, end_;
};

}  // namespace simulation

#endif  // SARP_SRC_SIMULATION_NETWORK_GENERATOR_TRAFIC_GENERATOR_H_
