//
// event_generator.cc
//

#include "network_generator/event_generator.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "structure/address.h"
#include "structure/packet.h"

namespace simulation {

EventGenerator::EventGenerator(Time start, Time end)
    : start_(start), end_(end) {}

TrafficGenerator::TrafficGenerator(Time start, Time end,
                                   std::vector<Node> &nodes, std::size_t count)
    : EventGenerator(start, end), nodes_(nodes), count_(count) {}

std::unique_ptr<Event> TrafficGenerator::Next() {
  if (counter_++ >= count_) {
    return nullptr;
  }
  std::size_t r1 = std::rand() % nodes_.size();
  std::size_t r2 = std::rand() % nodes_.size();
  if (r1 == r2) {  // Avoid reflexive traffic.
    r2 = (r2 + 1) % nodes_.size();
  }
  auto &sender = nodes_[r1];
  auto &reciever = nodes_[r2];
  Time t = start_ + std::rand() % (end_ - start_);
  uint32_t packet_size = 42;
  return std::make_unique<SendEvent>(t, TimeType::ABSOLUTE, sender, reciever,
                                     packet_size);
}

MoveGenerator::MoveGenerator(
    Time start, Time end, Time step_period, Network &network,
    std::unique_ptr<PositionGenerator> direction_generator, double min_speed,
    double max_speed, Time min_pause, Time max_pause)
    : EventGenerator(start, end),
      step_period_(step_period),
      network_(network),
      direction_generator_(std::move(direction_generator)),
      min_speed_(min_speed),
      max_speed_(max_speed),
      min_pause_(min_pause),
      max_pause_(max_pause),
      virtual_time_(start) {
  for (std::size_t i = 0; i < network_.get_nodes().size(); ++i) {
    plans_.emplace_back();
    CreatePlan(i);
    // Set initial position in that plan, create plan avoid this since this can
    // be used repeatedly after the plan is complete another is created and
    // initial position has to remain.
    plans_[i].current_position = network_.get_nodes()[i].get_position();
  }
}

std::unique_ptr<Event> MoveGenerator::Next() {
  while (true) {
    if (i_ >= plans_.size()) {
      i_ = 0;
      virtual_time_ += step_period_;
    }
    if (virtual_time_ >= end_) {
      return nullptr;
    }
    // Move according to plan.
    if (MakeStepInPlan(i_)) {
      // If plan is finished create a new one.
      CreatePlan(i_);
    }
    if (plans_[i_].is_paused) {
      // If node is paused dont create any event.
      ++i_;
      continue;
    } else {
      // New position changed, create appropriate Event.
      Node &node = network_.get_nodes()[i_];
      Position new_position = plans_[i_].current_position;
      ++i_;
      return std::make_unique<MoveEvent>(virtual_time_, TimeType::ABSOLUTE,
                                         node, network_, new_position);
    }
  }
}

double GetRandomDouble(double min, double max) {
  double f = (double)std::rand() / RAND_MAX;
  return min + f * (max - min);
}

void MoveGenerator::CreatePlan(std::size_t idx) {
  const auto [pos, success] = direction_generator_->Next();
  assert(success);
  plans_[idx].destination = pos;
  plans_[idx].pause =
      (max_pause_ <= min_pause_)
          ? max_pause_
          : std::rand() % (max_pause_ - min_pause_) + min_pause_;
  plans_[idx].speed = (max_speed_ <= min_speed_)
                          ? max_speed_
                          : GetRandomDouble(min_speed_, max_speed_);
}

static double NormalizeDouble(const double &d) {
  if (d > 0 && d < 1) {
    return 1;
  } else if (d > -1 && d < 0) {
    return -1;
  }
  return d;
}

// TODO: do this a little nicer
bool MoveGenerator::MakeStepInPlan(std::size_t idx) {
  if (plans_[idx].current_position == plans_[idx].destination &&
      plans_[idx].pause <= 0) {
    // We have reached the destination and waited for selected pause.
    return true;
  } else if (plans_[idx].current_position == plans_[idx].destination &&
             plans_[idx].pause > 0) {
    // Decrease the pause.
    plans_[idx].is_paused = true;
    plans_[idx].pause -= step_period_;
  } else {
    // Move in given direction with given speed and time step_period_.
    Position &pos = plans_[idx].current_position;
    Position &destination = plans_[idx].destination;

    double vector_x = destination.x - pos.x;
    double vector_y = destination.y - pos.y;
    double vector_z = destination.z - pos.z;
    double distance = std::sqrt(vector_x * vector_x + vector_y * vector_y +
                                vector_z * vector_z);
    double travel_distance = plans_[idx].speed * step_period_ / 1000;
    if (NormalizeDouble(travel_distance) > distance) {
      pos = destination;
    } else {
      double scale = travel_distance / distance;
      vector_x *= scale;
      vector_y *= scale;
      vector_z *= scale;
      pos.x += NormalizeDouble(vector_x);
      pos.y += NormalizeDouble(vector_y);
      pos.z += NormalizeDouble(vector_z);
    }
  }
  return false;
}

NeighborUpdateGenerator::NeighborUpdateGenerator(Time period, Time end,
                                                 Network &network)
    : EventGenerator(period, end),
      period_(period),
      virtual_time_(period),  // Start at first period.
      network_(network) {}

std::unique_ptr<Event> NeighborUpdateGenerator::Next() {
  if (virtual_time_ >= end_ || period_ == 0) {
    return nullptr;
  }
  auto event = std::make_unique<UpdateNeighborsEvent>(
      virtual_time_, TimeType::ABSOLUTE, network_);
  virtual_time_ += period_;
  return std::move(event);
}

CustomEventGenerator::CustomEventGenerator(
    std::vector<std::unique_ptr<Event>> events)
    : EventGenerator(0, 0), events_(std::move(events)) {}

std::unique_ptr<Event> CustomEventGenerator::Next() {
  if (events_.size() == 0) {
    return nullptr;
  }
  std::unique_ptr<Event> event = std::move(events_.back());
  events_.pop_back();
  return event;
}

}  // namespace simulation
