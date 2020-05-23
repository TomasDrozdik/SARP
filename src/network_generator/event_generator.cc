//
// event_generator.cc
//

#include "structure/event.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "structure/types.h"
#include "structure/packet.h"
#include "structure/network.h"
#include "sarp/routing.h"
#include "distance_vector/routing.h"

namespace simulation {

class BootEvent;

TrafficGenerator::TrafficGenerator(range<Time> time, Network &network, std::size_t count)
    : time_(time), network_(network), count_(count) {}

std::unique_ptr<Event> TrafficGenerator::Next() {
  if (count_-- == 0) {
    count_ = 0;  // deal with the overflow from the postfix operator
    return nullptr;
  }
  auto time = time_.first + std::rand() % (time_.second - time_.first);
  return std::make_unique<TrafficEvent>(time, TimeType::ABSOLUTE, network_);
}

MoveGenerator::MoveGenerator(range<Time> time, Time step_period,
    Network &network, std::unique_ptr<PositionGenerator> direction_generator,
    range<double> speed, range<Time> pause)
   : time_(time),
     step_period_(step_period),
     network_(network),
     direction_generator_(std::move(direction_generator)),
     speed_(speed),
     pause_(pause),
     virtual_time_(time.first) {
  for (std::size_t i = 0; i < network_.get_nodes().size(); ++i) {
    plans_.emplace_back();
    CreatePlan(i);
    // Set initial position in that plan, create plan avoid this since this can
    // be used repeatedly after the plan is complete another is created and
    // initial position has to remain.
    plans_[i].current_position = network_.get_nodes()[i]->get_position();
  }
}

std::unique_ptr<Event> MoveGenerator::Next() {
  return nullptr;
  //while (true) {
  //  if (i_ >= plans_.size()) {
  //    i_ = 0;
  //    virtual_time_ += step_period_;
  //  }
  //  if (virtual_time_ >= end_) {
  //    return nullptr;
  //  }
  //  // Move according to plan.
  //  if (MakeStepInPlan(i_)) {
  //    // If plan is finished create a new one.
  //    CreatePlan(i_);
  //  }
  //  if (plans_[i_].is_paused) {
  //    // If node is paused dont create any event.
  //    ++i_;
  //    continue;
  //  } else {
  //    // New position changed, create appropriate Event.
  //    Node &node = *network_.get_nodes()[i_];
  //    Position new_position = plans_[i_].current_position;
  //    ++i_;
  //    return std::make_unique<MoveEvent>(virtual_time_, TimeType::ABSOLUTE,
  //                                       node, network_, new_position);
  //  }
  //}
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
      (pause_.second <= pause_.first)
          ? pause_.second
          : std::rand() % (pause_.second - pause_.first) + pause_.first;
  plans_[idx].speed = (speed_.second <= speed_.first)
                          ? speed_.second
                          : GetRandomDouble(speed_.first, speed_.second);
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

NeighborUpdateGenerator::NeighborUpdateGenerator(range<Time> time, Time period,
                                                 Network &network)
    : time_(time),
      period_(period),
      virtual_time_(time_.first),  // Start at first period.
      network_(network) {}

std::unique_ptr<Event> NeighborUpdateGenerator::Next() {
  if (virtual_time_ >= time_.second || period_ == 0) {
    return nullptr;
  }
  auto event = std::make_unique<UpdateNeighborsEvent>(
      virtual_time_, TimeType::ABSOLUTE, network_);
  virtual_time_ += period_;
  return std::move(event);
}

CustomEventGenerator::CustomEventGenerator(
    std::vector<std::unique_ptr<Event>> events)
    : events_(std::move(events)) {}

std::unique_ptr<Event> CustomEventGenerator::Next() {
  if (events_.size() == 0) {
    return nullptr;
  }
  std::unique_ptr<Event> event = std::move(events_.back());
  events_.pop_back();
  return event;
}

NodeGenerator::NodeGenerator(Network &network, std::size_t count,
    RoutingType routing,
    std::unique_ptr<TimeGenerator> time_generator,
    std::unique_ptr<PositionGenerator> pos_generator,
    std::unique_ptr<AddressGenerator> address_generator)
  : network_(network), count_(count), routing_(routing), 
    time_generator_(std::move(time_generator)),
    pos_generator_(std::move(pos_generator)),
    address_generator_(std::move(address_generator)) {
  assert(time_generator_); 
  assert(pos_generator_); 
}

std::unique_ptr<Event> NodeGenerator::Next() {
  if (count_-- == 0) {
    count_ = 0;
    return nullptr;
  }
  auto node = std::make_unique<Node>();
  auto [pos, pos_success] = pos_generator_->Next();
  assert(pos_success);
  node->set_position(pos);
  if (address_generator_) {
    auto [address, address_success] = address_generator_->Next(pos);
    assert(address_success);
    node->AddAddress(address);
  }
  switch (routing_) {
    case RoutingType::DISTANCE_VECTOR:
      node->set_routing(std::make_unique<DistanceVectorRouting>(*node));
      break;
    case RoutingType::SARP:
      node->set_routing(std::make_unique<SarpRouting>(*node));
      break;
    default:
      assert(false);
  }
  assert(node->IsInitialized());
  auto [boot_time, time_success] = time_generator_->Next();
  assert(time_success);
  return std::make_unique<BootEvent>(boot_time, TimeType::ABSOLUTE, network_,
                                     std::move(node));
}

}  // namespace simulation
