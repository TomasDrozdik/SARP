//
// simulation.cc
//

#define PRINT

#include "structure/simulation.h"

#include <algorithm>
#include <limits>

#include "structure/simulation_parameters.h"
#include "structure/statistics.h"

namespace simulation {

Simulation& Simulation::get_instance() {
  if (instance_ == nullptr) {
    instance_ = new Simulation();
  }
  return *instance_;
}

bool Simulation::EventComparer::operator()(const std::unique_ptr<Event> &t1,
    const std::unique_ptr<Event> &t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

void Simulation::Run(std::unique_ptr<Network> network,
    std::vector<std::unique_ptr<EventGenerator>> &event_generators) {
  // TODO: move to function or comment better.
  network_ = std::move(network);

  // Check if duration, ttl_limit and connection_speed are initialized.
  if (!SimulationParameters::IsMandatoryInitialized()) {
    std::cerr << "Simulation Parameters uninitialized\n";
  }
  for (auto &generator : event_generators) {
    for (std::unique_ptr<Event> event = ++(*generator);
        event != nullptr; event = ++(*generator)) {
      ScheduleEvent(std::move(event));
    }
  }
#ifdef PRINT
  SimulationParameters::Print(std::cout);
  std::cout << "\n___________BEGIN____________\ntime:event:description\n";
#endif
  for (time_ = 0; time_ < SimulationParameters::get_duration(); ++time_) {
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
  std::cout << "____________END_____________\n\n";
  Statistics::Print(std::cout, *network_);
}

void Simulation::ScheduleEvent(std::unique_ptr<Event> event) {
  if (event->IsRelativeTime()) {
    event->time_ += this->time_;
  }
  schedule_.push(std::move(event));
}

}  // namespace simulation
