//
// address.cc
//

#include <iostream>

#include "address.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Address &addr) {
  return addr.Print(os);
}

Address::~Address() { }

SimpleAddress::SimpleAddress(uint32_t addr) : addr_(addr) { }

SimpleAddress::SimpleAddress(const SimpleAddress& other) :
    addr_(other.addr_) { }

std::unique_ptr<Address> SimpleAddress::Clone() const {
  return std::make_unique<SimpleAddress>(*this);
}

std::size_t SimpleAddress::Hash() const {
  return std::hash<uint32_t>()(addr_);
}

std::ostream &SimpleAddress::Print(std::ostream &os) const {
  return os << addr_;
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

}  // namespace simulation
