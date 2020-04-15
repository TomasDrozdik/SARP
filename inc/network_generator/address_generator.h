//
// address_iterator.h
//

#ifndef SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
#define SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_

#include <limits.h>
#include <memory.h>

#include "structure/address.h"
#include "structure/position.h"

namespace simulation {

class AddressGenerator {
 public:
  virtual std::pair<Address, bool> Next(Position pos) = 0;
};

class SequentialAddressGenerator final : public AddressGenerator {
 public:
  std::pair<Address, bool> Next(Position) {
    GenerateNext();
    return std::make_pair(next_address_, true);
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

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
