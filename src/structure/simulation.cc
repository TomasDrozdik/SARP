//
// simulation.cc
//

#include "structure/simulation.h"

#include <algorithm>
#include <limits>

namespace simulation {

std::pair<std::unique_ptr<Network>,
          std::vector<std::unique_ptr<EventGenerator>>>
Simulation::CreateScenario(const Parameters &p) {
  assert(p.has_general() && "No general parameters present.");
  auto network = std::make_unique<Network>();
  std::vector<std::unique_ptr<EventGenerator>> event_generators;
  if (p.has_node_generation()) {
    assert(p.get_node_generation().initial_positions);
    const auto &initial_addresses = p.get_node_generation().initial_addresses;
    event_generators.push_back(std::make_unique<NodeGenerator>(
        *network,
        p.get_node_generation().node_count,
        p.get_node_generation().routing_type,
        std::make_unique<TimeGenerator>(),
        p.get_node_generation().initial_positions->Clone(),
        initial_addresses ? initial_addresses->Clone() : nullptr));
  }

  event_generators.push_back(std::make_unique<NeighborUpdateGenerator>(
      range<Time>{0, p.get_general().duration},
      p.get_general().neighbor_update_period,
      *network));

  if (p.has_traffic()) {
    event_generators.push_back(std::make_unique<TrafficGenerator>(
        p.get_traffic().time_range,
        *network,
        p.get_traffic().event_count));
  }
  return std::make_pair(std::move(network), std::move(event_generators));
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
                                           const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

void Simulation::Run(Env &env, Network &network, unsigned seed,
                     std::vector<std::unique_ptr<EventGenerator>> &events) {
  std::srand(seed);
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
}

void Simulation::Start(Env &env, Network &network) {
  assert(&env.simulation == this);

  // Begin the event loop.
#define PRINT
#ifdef PRINT
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < env.parameters.get_general().duration; ++time_) {
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
#ifdef PRINT
  std::cout << "____________END_____________\n\n";
#endif

//#define CSV
#ifdef CSV
  env.parameters.PrintCsv(std::cout);
  env.stats.PrintCsv(std::cout, network);
#else
  std::cout << env.parameters;
  env.stats.Print(std::cout, network);
  std::cout << '\n';
#endif
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->IsRelativeTime()) {
    event->time_ += this->time_;
  }
  schedule_.push(std::move(event));
}

}  // namespace simulation
