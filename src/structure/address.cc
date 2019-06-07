//
// address.cc
//

#include <iostream>

#include "address.h"

namespace simulation {

Address::~Address() { }

SimpleAddress::SimpleAddress(uint32_t addr) : addr_(addr) { }

SimpleAddress::SimpleAddress(const SimpleAddress& other) :
    addr_(other.addr_) { }

std::unique_ptr<Address> SimpleAddress::Clone() const {
  return std::make_unique<SimpleAddress>(*this);
}

SimpleAddress::operator std::string() const {
  return std::to_string(addr_);
}

bool SimpleAddress::operator==(const Address &other) const {
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
