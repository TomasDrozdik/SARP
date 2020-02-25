//
// address_iterator.h
//

#ifndef SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
#define SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_

#include <memory.h>

#include "../structure/address.h"
#include "../structure/position.h"

namespace simulation {

template <class AddressType>
class AddressIterator {
 public:
  // This class should not be used. Provide proper template specialization.
  AddressIterator() = delete;

  std::unique_ptr<AddressType> GenerateAddress(Position pos = {0,0,0}) {
    return nullptr;
  }
};

template <>
class AddressIterator<SimpleAddress> {
 public:
  AddressIterator() = default;

  std::unique_ptr<SimpleAddress> GenerateAddress(Position) {
    return std::make_unique<SimpleAddress>(i++);
  }

 private:
  uint32_t i = 0;
};

}  // namespace simulation

#endif  // SARP_NETWORK_GENERATOR_ADDRESS_ITERATOR_H_
