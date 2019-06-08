//
// event_generator.cc
//

#include "event_generator.h"

#include <cstdlib>

#include "../structure/address.h"
#include "../structure/protocol_packet.h"

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
  auto sender = nodes_[r1].get();
  auto reciever = nodes_[r2].get();
  Time t = start_ + std::rand() % (end_ - start_);
  auto packet = std::make_unique<ProtocolPacket>(
    sender->get_address()->Clone(), reciever->get_address()->Clone());
  return std::make_unique<SendEvent>(t, *sender, std::move(packet));
}

MoveGenerator::MoveGenerator(Time start, Time end,
		const Network &incomplete_network) : EventGenerator(start, end) {
	// TODO
}

std::unique_ptr<Event> MoveGenerator::operator++() {
	// TODO
	return nullptr;
}

}  // namespace simulation