//
// trafic_generator.cc
//

#include "trafic_generator.h"

#include <cstdlib>

#include "../structure/address.h"
#include "../structure/protocol_packet.h"

namespace simulation {

TraficGenerator::TraficGenerator(
    const std::vector<std::unique_ptr<Node>> &nodes, Time start, Time end) :
        nodes_(nodes), start_(start), end_(end) { }

std::unique_ptr<Event> TraficGenerator::operator++() {
  auto sender = nodes_[std::rand() % nodes_.size()].get();
  auto reciever = nodes_[std::rand() % nodes_.size()].get();
  Time t = start_ + std::rand() % (end_ - start_);
  auto packet = std::make_unique<ProtocolPacket>(
    sender->get_address()->Clone(), reciever->get_address()->Clone());
  return std::make_unique<SendEvent>(t, *sender, std::move(packet));
}

}  // namespace simulation
