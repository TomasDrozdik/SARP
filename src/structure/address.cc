//
// address.cc
//

#include <iostream>

#include "structure/address.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const Address &addr) {
  return addr.Print(os);
}

Address::~Address() { }

std::size_t AddressHash::operator()(const std::unique_ptr<Address> &addr)
    const {
  return addr->Hash();
}

bool AddressComparer::operator()(const std::unique_ptr<Address> &a1,
    const std::unique_ptr<Address> &a2) const {
  return *a1 == *a2;
}

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

bool SimpleAddress::operator==(const Address &other) const {
  return addr_ == dynamic_cast<const SimpleAddress &>(other).addr_;
}

bool SimpleAddress::operator<(const Address &other) const {
  return addr_ < dynamic_cast<const SimpleAddress &>(other).addr_;
}

}  // namespace simulation
