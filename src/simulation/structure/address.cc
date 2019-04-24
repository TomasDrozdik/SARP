//
// address.cc
//

#include "address.h"

namespace simulation {

SimpleAddress::SimpleAddress(uint32_t addr) : addr_(addr) { }

SimpleAddress::SimpleAddress(const SimpleAddress& other) :
    addr_(other.addr_) { }

bool SimpleAddress::operator==(const Address &other) {
  return addr_ == dynamic_cast<const SimpleAddress&>(other).addr_;
}

bool SimpleAddress::operator<(const SimpleAddress &other) const {
  return addr_ < other.addr_;
}

int SimpleAddress::get_length() const {
  return sizeof (addr_);
}

uint32_t SimpleAddress::get_address() const {
  return addr_;
}

}  // namespace simulation