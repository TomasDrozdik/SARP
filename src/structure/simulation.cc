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

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
Simulation::CreateScenario(const Parameters &p) {
  std::unique_ptr<Network> network = nullptr;
  NetworkGenerator<StaticRouting> static_ng;
  NetworkGenerator<DistanceVectorRouting> dv_ng;
  NetworkGenerator<SarpRouting> sarp_ng;
  switch (p.get_routing_type()) {
    case RoutingType::STATIC:
      network =
          static_ng.Create(p.get_node_count(), p.get_initial_positions(),
                           std::make_unique<SequentialAddressGenerator>());
      break;
    case RoutingType::DISTANCE_VECTOR:
      network = dv_ng.Create(p.get_node_count(), p.get_initial_positions(),
                             std::make_unique<SequentialAddressGenerator>());
      break;
    case RoutingType::SARP:
      network = sarp_ng.Create(p.get_node_count(), p.get_initial_positions(),
                               std::make_unique<SequentialAddressGenerator>());
      break;
    default:
      assert(false);
  }

  std::vector<std::unique_ptr<EventGenerator>> event_generators;

  if (p.has_traffic()) {
    event_generators.push_back(std::make_unique<TrafficGenerator>(
        p.get_traffic_time_range().first, p.get_traffic_time_range().second,
        network->get_nodes(), p.get_traffic_event_count()));
  }
  if (p.has_movement()) {
    event_generators.push_back(std::make_unique<MoveGenerator>(
        p.get_move_time_range().first, p.get_move_time_range().second,
        p.get_move_step_period(), *network, p.get_move_directions(),
        p.get_move_speed_range().first, p.get_move_speed_range().second,
        p.get_move_pause_range().first, p.get_move_pause_range().second));

    event_generators.push_back(std::make_unique<NeighborUpdateGenerator>(
        p.get_neighbor_update_period(), p.get_duration(), *network));
  }
  return std::make_pair(std::move(network), std::move(event_generators));
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
                                           const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

void Simulation::Run(Env &env, Network &network,
                     std::vector<std::unique_ptr<EventGenerator>> &events) {
  env.stats.Reset();
  env.simulation.InitSchedule(network, events);
  env.simulation.Start(env, network);
}

void Simulation::InitSchedule(
    Network &network, std::vector<std::unique_ptr<EventGenerator>> &events) {
  assert(schedule_.empty());
  // Generate all events passed form event generators.
  for (auto &generator : events) {
    for (std::unique_ptr<Event> event = generator->Next(); event != nullptr;
         event = generator->Next()) {
      ScheduleEvent(std::move(event));
    }
  }
  // Add network initialization event.
  ScheduleEvent(
      std::make_unique<InitNetworkEvent>(0, TimeType::ABSOLUTE, network));
}

void Simulation::Start(Env &env, Network &network) {
  assert(&env.simulation == this);

  // Begin the event loop.
#ifdef PRINT
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < env.parameters.get_duration(); ++time_) {
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
      event->Execute(env);
    }
  }
  std::cout << "____________END_____________\n\n" << env.parameters << '\n';
  env.stats.Print(std::cout, network);
  std::cout << '\n';
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->IsRelativeTime()) {
    event->time_ += this->time_;
  }
  schedule_.push(std::move(event));
}

}  // namespace simulation
