//
// main.cc
//
#include <map>

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "../network_generator/network_generator.h"
#include "../network_generator/position_generator.h"
#include "../network_generator/trafic_generator.h"
#include "../network_generator/address_iterator.h"
#include "../structure/event.h"
#include "../structure/network.h"
#include "../structure/node.h"
#include "../structure/position.h"
#include "../structure/simulation.h"
#include "../structure/wireless_connection.h"
#include "../static_forwarding/static_routing.h"

using namespace simulation;

int main() {
  NetworkGenerator<SimpleAddress, StaticRouting, WirelessConnection>
      network_generator;
  PositionGenerator pos_generator{0, 10, 0, 10, 0, 0};
  auto network = network_generator.Create(3, pos_generator);
  Time trafic_start = 0;
  Time trafic_end = 400;
  TraficGenerator trafic_generator(network->get_nodes(), trafic_start,
      trafic_end);
  auto events = std::vector<std::unique_ptr<Event>>();
  for (size_t i = 0; i < 10; ++i) {
    events.push_back(++trafic_generator);
  }

  Time simulation_duration = 500;
  uint32_t signal_speed_Mbps = 433;  // 802.11ac

  Simulation::set_instance(std::move(network), simulation_duration,
      signal_speed_Mbps);
  Simulation::get_instance().Run(std::move(events));
  return 0;
}