//
// simulation.cc
//

#define PRINT
#include "structure/simulation.h"

#include <algorithm>
#include <limits>

#include "distance_vector/routing.h"
#include "network_generator/address_generator.h"
#include "network_generator/network_generator.h"
#include "network_generator/position_generator.h"
#include "sarp/routing.h"
#include "static_routing/routing.h"
#include "structure/statistics.h"

extern std::unique_ptr<simulation::SimulationParameters> config;

namespace simulation {

Simulation &Simulation::get_instance() {
  if (instance_ == nullptr) {
    instance_ = new Simulation();
  }
  return *instance_;
}

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
Simulation::CreateScenario() {
  assert(config != nullptr && "Config has to be initialized to run this.");
  std::unique_ptr<Network> network = nullptr;
  NetworkGenerator<StaticRouting> static_ng;
  NetworkGenerator<DistanceVectorRouting> dv_ng;
  NetworkGenerator<SarpRouting> sarp_ng;
  switch (config->routing_type) {
    case RoutingType::STATIC:
      network =
          static_ng.Create(config->node_count, config->get_initial_positions(),
                           SequentialAddressGenerator());
      break;
    case RoutingType::DISTANCE_VECTOR:
      network =
          dv_ng.Create(config->node_count, config->get_initial_positions(),
                       SequentialAddressGenerator());
      break;
    case RoutingType::SARP:
      network =
          sarp_ng.Create(config->node_count, config->get_initial_positions(),
                         SequentialAddressGenerator());
      break;
    default:
      assert(false);
  }

  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  if (config->traffic_start < config->traffic_end &&
      config->traffic_event_count > 0) {
    event_generators.push_back(std::make_unique<TrafficGenerator>(
        config->traffic_start, config->traffic_end, network->get_nodes(),
        config->traffic_event_count));
  }
  if (config->move_start < config->move_end) {
    event_generators.push_back(std::make_unique<MoveGenerator>(
        config->move_start, config->move_end, config->move_step_period,
        *network, config->get_move_directions(), config->move_speed_min,
        config->move_speed_max, config->move_pause_min,
        config->move_pause_max));
  }
  if (config->neighbor_update_period >= 0) {
    event_generators.push_back(
        std::make_unique<NeighborPeriodicUpdateGenerator>(
            config->neighbor_update_period, config->duration, *network));
  }
  return std::make_pair(std::move(network), std::move(event_generators));
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
                                           const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

void Simulation::Run(
    std::unique_ptr<Network> network,
    std::vector<std::unique_ptr<EventGenerator>> event_generators) {
  // TODO: move to function or comment better.
  network_ = std::move(network);

  // Generate all events passed form event generators.
  for (auto &generator : event_generators) {
    for (std::unique_ptr<Event> event = generator->Next(); event != nullptr;
         event = generator->Next()) {
      ScheduleEvent(std::move(event));
    }
  }
  // Add network initialization event.
  ScheduleEvent(
      std::make_unique<InitNetworkEvent>(0, TimeType::ABSOLUTE, *network_));
  // Begin the event loop.
#ifdef PRINT
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < config->duration; ++time_) {
    while (!schedule_.empty() && schedule_.top()->time_ <= time_) {
      // Extract event from the schedule.
      // HACK: Using const_cast to extract the Event from the priority_queue
      // before we remove it by pop().
      // This is because event.Execute() can add an event to the top of the
      // priority_queue and than we would pop() the wrong one after an execute
      // leaving the already Executed one in the priority_queue.
      std::unique_ptr<Event> event =
          std::move(const_cast<std::unique_ptr<Event> &>(schedule_.top()));
      schedule_.pop();
#ifdef PRINT
      event->Print(std::cout);
#endif
      event->Execute();
    }
  }
  std::cout << "____________END_____________\n\n" << *config << '\n';
  Statistics::Print(std::cout, *network_);
  std::cout << '\n';
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->IsRelativeTime()) {
    event->time_ += this->time_;
  }
  schedule_.push(std::move(event));
}

void Simulation::ExportNetworkToDot(std::ostream &os) const {
  assert(network_);
  network_->ExportToDot(os);
}

}  // namespace simulation
