//
// event_generator.cc
//

#include "network_generator/event_generator.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "structure/address.h"
#include "structure/protocol_packet.h"

double NormalizeDouble(const double &d) {
  if (d > 0 && d < 1) {
    return 1;
  } else if (d > -1 && d < 0) {
    return -1;
  }
  return d;
}

namespace simulation {

EventGenerator::EventGenerator(Time start, Time end) :
    start_(start), end_(end) { }

TraficGenerator::TraficGenerator(Time start, Time end,
    const std::vector<std::unique_ptr<Node>> &nodes,
    std::size_t count, bool reflexive_trafic) :
        EventGenerator(start, end), nodes_(nodes), count_(count),
        reflexive_trafic_(reflexive_trafic) { }

std::unique_ptr<Event> TraficGenerator::operator++() {
  if (counter_++ >= count_) {
    return nullptr;
  }
  std::size_t r1 = std::rand() % nodes_.size();
  std::size_t r2 = std::rand() % nodes_.size();
  if (!reflexive_trafic_ && r1 == r2) {
    r2 = (r2 + 1) % nodes_.size();
  }
  auto &sender = nodes_[r1];
  auto &reciever = nodes_[r2];
  Time t = start_ + std::rand() % (end_ - start_);
  uint32_t packet_size = 42;
  return std::make_unique<SendEvent>(t, TimeType::ABSOLUTE, *sender, *reciever, packet_size);
}

MoveGenerator::MoveGenerator(Time start, Time end, Time step_period,
    const Network &network, PositionGenerator &direction_generator,
    double min_speed, double max_speed,
    Time min_pause, Time max_pause) :
        EventGenerator(start, end), step_period_(step_period),
        network_(network), direction_generator_(direction_generator),
        min_speed_(min_speed), max_speed_(max_speed),
        min_pause_(min_pause), max_pause_(max_pause),
        virtual_time_(start) {
  for (std::size_t i = 0; i < network_.get_nodes().size(); ++i) {
    plans_.emplace_back();
    CreatePlan(i);
    // Set initial position in that plan.
    plans_[i].current = network_.get_nodes()[i]->get_connection().position;
  }
}

std::unique_ptr<Event> MoveGenerator::operator++() {
  while (true) {
    if (i_ >= plans_.size()) {
      i_ = 0;
      virtual_time_ += step_period_;
    }
    if (virtual_time_ >= end_) {
      return nullptr;
    }
    if (MakeStepInPlan(i_)) {
      CreatePlan(i_);
    }
    // Check if node is paused
    if (plans_[i_].is_paused) {
      ++i_;
      continue;
    } else {
      ++i_;
      change_ = true;
      return std::make_unique<MoveEvent>(virtual_time_, TimeType::ABSOLUTE,
          *network_.get_nodes()[i_ - 1], plans_[i_ - 1].current);
    }
  }
}

double GetRandomDouble(double min, double max) {
  double f = (double)std::rand() / RAND_MAX;
  return min + f * (max - min);
}

void MoveGenerator::CreatePlan(std::size_t idx) {
  plans_[idx].destination = ++direction_generator_;
  plans_[idx].pause = (max_pause_ == min_pause_) ? max_pause_ :
      std::rand() % (max_pause_ - min_pause_) + min_pause_;
  plans_[idx].speed = (max_speed_ == min_speed_) ? max_speed_ :
      GetRandomDouble(min_speed_, max_speed_);
}

// TODO: do this a little nicer
bool MoveGenerator::MakeStepInPlan(std::size_t idx) {
  if (plans_[idx].current == plans_[idx].destination &&
      plans_[idx].pause <= 0) {
    return true;
  } else if (plans_[idx].current == plans_[idx].destination &&
      plans_[idx].pause > 0) {
    // Decrease the pause
    plans_[idx].is_paused = true;
    plans_[idx].pause -= step_period_;
  } else {
    // Move in given direction with given speed and time step_period_
    Position &pos = plans_[idx].current;
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

RoutingPeriodicUpdateGenerator::RoutingPeriodicUpdateGenerator(Time start,
    Time end, Time period, Network &network) :
        EventGenerator(start, end), period_(period), network_(network),
        virtual_time_(start) { }

std::unique_ptr<Event> RoutingPeriodicUpdateGenerator::operator++() {
  if (update_interfaces) {
    update_interfaces = false;
    return std::make_unique<UpdateInterfacesEvent>(virtual_time_,
        TimeType::ABSOLUTE, network_);
  }
  if (i_ >= network_.get_nodes().size() && j_ >= network_.get_nodes().size()) {
    i_ = 0;
    j_ = 0;
    virtual_time_ += period_;
  }
  if (virtual_time_ >= end_) {
    return nullptr;
  }
  if (i_ < network_.get_nodes().size()) {
    // Schedule these at time + 1 to make these events happen after interface
    // update.
    return std::make_unique<UpdateRoutingInterfacesEvent>(virtual_time_ + 1,
        TimeType::ABSOLUTE, network_.get_nodes()[i_++]->get_routing());
  } else if (j_ < network_.get_nodes().size()) {
    // Schedule these at time + 2 to make these events happen after interface
    // update on routing.
    return std::make_unique<UpdateRoutingEvent>(virtual_time_ + 2, TimeType::ABSOLUTE,
        network_.get_nodes()[j_++]->get_routing());
  }
  assert(false);
}

}  // namespace simulation
