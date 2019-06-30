//
// sarp_address.cc
//

#include "address.h"

namespace simulation {

SarpAddress::SarpAddress(const std::vector<unsigned char> &data) :
    data_(data) { }

SarpAddress::SarpAddress(std::vector<unsigned char> &&data) :
    data_(data) { }

std::unique_ptr<Address> SarpAddress::Clone() const {
  return std::make_unique<SarpAddress>(data_);
}

std::size_t SarpAddress::Hash() const {
  std::hash<AddressComponentType> hasher;
  std::size_t hash_value = hasher(data_[0]);

  for (std::size_t i = 1; i < data_.size(); ++i) {
    // https://stackoverflow.com/a/2595226
    hash_value ^= hasher(data_[i]) + 0x9e3779b9 +
        (hash_value<<6) + (hash_value>>2);
  }
  return hash_value;
}

std::ostream &SarpAddress::Print(std::ostream &os) const {
  std::size_t i;
  for (i = 0; i < data_.size() - 1; ++i) {
    os << data_[i] << '.';
  }
  return os << data_[i];
}

bool SarpAddress::operator==(const Address &other) const {
  auto &other_data = dynamic_cast<const SarpAddress &>(other).data_;
  if (data_.size() != other_data.size()) {
    return false;
  }
  for (std::size_t i = 0; i < data_.size(); ++i) {
    if (data_[i] != other_data[i]) {
      return false;
    }
  }
  return true;
}

}  // namespace simulation
