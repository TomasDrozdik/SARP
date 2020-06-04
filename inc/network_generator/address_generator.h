//
// address_iterator.h
//

#ifndef SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
#define SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_

#include <limits>
#include <memory>

#include "structure/position.h"
#include "structure/types.h"

namespace simulation {

class AddressGenerator {
 public:
  virtual std::pair<Address, bool> Next(Position pos) = 0;
  virtual std::unique_ptr<AddressGenerator> Clone() = 0;
};

class SequentialAddressGenerator final : public AddressGenerator {
 public:
  std::pair<Address, bool> Next(Position) {
    GenerateNext();
    return std::make_pair(next_address_, true);
  }

  std::unique_ptr<AddressGenerator> Clone() override {
    auto p = std::make_unique<SequentialAddressGenerator>();
    p->next_address_ = next_address_;
    return std::move(p);
  }

 private:
  void GenerateNext() {
    if (next_address_.empty() ||
        next_address_.back() == std::numeric_limits<unsigned char>::max()) {
      next_address_.push_back(0);
    } else {
      ++next_address_.back();
    }
  }

  Address next_address_;
};

class BinaryAddressGenerator final : public AddressGenerator {
 public:
  std::pair<Address, bool> Next(Position) {
    GenerateNext();
    return std::make_pair(next_address_, true);
  }

  std::unique_ptr<AddressGenerator> Clone() override {
    auto p = std::make_unique<BinaryAddressGenerator>();
    p->next_address_ = next_address_;
    return std::move(p);
  }

 private:
  void GenerateNext() {
    if (next_address_.empty() || next_address_.back() == 1) {
      next_address_.push_back(0);
    } else {
      ++next_address_.back();
    }
  }

  Address next_address_ = {0};
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
