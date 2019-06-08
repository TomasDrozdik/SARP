//
// event_generator.h
//

#ifndef SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
#define SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_

#include <vector>
#include <memory>

#include "../structure/event.h"
#include "../structure/network.h"
#include "../structure/node.h"

namespace simulation {

class Event;

class EventGenerator {
 public:
  virtual ~EventGenerator() = default;

  // RETURNS: pointer to new Event or nullptr if generation ends.
  virtual std::unique_ptr<Event> operator++() = 0;
 protected:
  EventGenerator(Time start, Time end);
  Time start_, end_;
};

class TraficGenerator : public EventGenerator {
 public:
  TraficGenerator(Time start, Time end,
      const std::vector<std::unique_ptr<Node>> &nodes,
      std::size_t count, bool reflexive_trafic = true);
  ~TraficGenerator() override = default;

  // Create new send event form random time in time interval and between random
  // two nodes.
  std::unique_ptr<Event> operator++() override;
 private:
  const std::vector<std::unique_ptr<Node>> &nodes_;
  std::size_t count_;
  bool reflexive_trafic_;
  std::size_t counter_ = 0;
};

class MoveGenerator : public EventGenerator {
 public:
  MoveGenerator(Time start, Time end, const Network &incomplete_network);
  ~MoveGenerator() override = default;

  std::unique_ptr<Event> operator++() override;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_EVENT_GENERATOR_H_
