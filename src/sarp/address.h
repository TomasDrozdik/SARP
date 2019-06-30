//
// sarp_address.h
//

#ifndef SARP_SARP_ADDRESS_H_
#define SARP_SARP_ADDRESS_H_

#include "../network_generator/address_generator.h"
#include "../structure/address.h"

#include <memory>
#include <vector>

namespace simulation {

class SarpAddress final : public Address {
 public:
  using AddressComponentType = unsigned char;

  SarpAddress(const std::vector<unsigned char> &data);
  SarpAddress(std::vector<unsigned char> &&data);

  ~SarpAddress() override = default;

  std::unique_ptr<Address> Clone() const override;

  std::size_t Hash() const override;

  std::ostream &Print(std::ostream &os) const override;

  bool operator==(const Address &other) const override;
  bool operator<(const Address &other) const override;

  std::vector<AddressComponentType> data_;
};

template <>
class AddressIterator<SarpAddress> {
 public:

  std::unique_ptr<SarpAddress> GenerateAddress(Position pos) {
    return std::make_unique<SarpAddress>(
        std::vector<SarpAddress::AddressComponentType>(
            {static_cast<SarpAddress::AddressComponentType>(pos.x),
            static_cast<SarpAddress::AddressComponentType>(pos.y),
            static_cast<SarpAddress::AddressComponentType>(pos.z)}));
  }
};


}  // namespace simulation

#endif  // SARP_SARP_ADDRESS_H_
