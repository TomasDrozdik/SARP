//
// event_generator.cc
//

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "distance_vector/routing.h"
#include "sarp/routing.h"
#include "structure/event.h"
#include "structure/network.h"
#include "structure/packet.h"
#include "structure/types.h"

namespace simulation {

class BootEvent;

RandomTrafficGenerator::RandomTrafficGenerator(range<Time> time,
                                               Network &network,
                                               std::size_t count)
    : time_(time), network_(network), count_(count) {}

std::unique_ptr<Event> RandomTrafficGenerator::Next() {
  if (count_-- == 0) {
    count_ = 0;  // deal with the overflow from the postfix operator
    return nullptr;
  }
  auto time = time_.first + std::rand() % (time_.second - time_.first);
  return std::make_unique<RandomTrafficEvent>(time, TimeType::ABSOLUTE,
                                              network_);
}

SpecificTrafficGenerator::SpecificTrafficGenerator(range<Time> time,
                                                   Network &network,
                                                   std::size_t count,
                                                   range<NodeID> from,
                                                   range<NodeID> to)
    : time_(time), network_(network), count_(count), from_(from), to_(to) {
  assert(from.second > from.first && "Specify interval [x, y) && x < y ");
  assert(to.second > to.first && "Specify interval [x, y) && x < y ");
}

std::unique_ptr<Event> SpecificTrafficGenerator::Next() {
  if (count_-- == 0) {
    count_ = 0;  // deal with the overflow from the postfix operator
    return nullptr;
  }
  auto time = time_.first + std::rand() % (time_.second - time_.first);
  std::size_t idx_from = std::rand() % (from_.second - from_.first);
  std::size_t idx_to = std::rand() % (to_.second - to_.first);
  return std::make_unique<TrafficEvent>(time, TimeType::ABSOLUTE, network_,
                                        from_.first + idx_from,
                                        to_.first + idx_to);
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

NodeGenerator::NodeGenerator(
    Network &network, std::size_t count, RoutingType routing,
    std::unique_ptr<TimeGenerator> time_generator,
    std::unique_ptr<PositionGenerator> pos_generator,
    std::unique_ptr<AddressGenerator> address_generator)
    : network_(network),
      count_(count),
      routing_(routing),
      time_generator_(std::move(time_generator)),
      pos_generator_(std::move(pos_generator)),
      address_generator_(std::move(address_generator)) {
  assert(time_generator_);
  assert(pos_generator_);
}

std::unique_ptr<BootEvent> NodeGenerator::CreateBootEvent(
    Time time, TimeType time_type, Network &network, RoutingType routing,
    Position position, Address address,
    std::unique_ptr<PositionGenerator> directions) {
  auto node = std::make_unique<Node>();
  switch (routing) {
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

  node->set_position(position);

  if (address.size() != 0) {
    node->AddAddress(address);
  }
  return std::make_unique<BootEvent>(time, time_type, network, std::move(node),
                                     std::move(directions));
}

std::unique_ptr<Event> NodeGenerator::Next() {
  if (count_-- == 0) {
    count_ = 0;
    return nullptr;
  }
  auto [pos, pos_success] = pos_generator_->Next();
  assert(pos_success);

  Address address{};  // empty address
  if (address_generator_) {
    auto [generated_address, address_success] = address_generator_->Next(pos);
    assert(address_success);
    address = generated_address;
  }

  auto [boot_time, time_success] = time_generator_->Next();
  assert(time_success);

  std::unique_ptr<PositionGenerator> directions = nullptr;
  return CreateBootEvent(boot_time, TimeType::ABSOLUTE, network_, routing_, pos,
                         address, std::move(directions));
}

ReaddressEventGenerator::ReaddressEventGenerator(range<Time> time, Time period,
                                                 Network &network)
    : time_(time),
      period_(period),
      virtual_time_(time_.first),  // Start at first period.
      network_(network) {}

std::unique_ptr<Event> ReaddressEventGenerator::Next() {
  if (virtual_time_ >= time_.second || period_ == 0) {
    return nullptr;
  }
  bool only_empty = false;
  auto event = std::make_unique<ReaddressEvent>(
      virtual_time_, TimeType::ABSOLUTE, network_, only_empty);
  virtual_time_ += period_;
  return std::move(event);
}

}  // namespace simulation
