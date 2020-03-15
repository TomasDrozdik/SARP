//
// sarp_address.h
//

#ifndef SARP_SARP_ADDRESS_H_
#define SARP_SARP_ADDRESS_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "network_generator/address_generator.h"
#include "structure/address.h"
#include "structure/network.h"

namespace simulation {

class SarpAddress final : public Address {
 public:
  using AddressComponentType = unsigned char;

  SarpAddress(const std::vector<unsigned char> &data);
  SarpAddress(std::vector<unsigned char> &&data);

  ~SarpAddress() override = default;

  static void AssignAddresses(Network &network);

  std::unique_ptr<Address> Clone() const override;

  std::size_t Hash() const override;

  std::ostream &Print(std::ostream &os) const override;

  bool operator==(const Address &other) const override;
  bool operator<(const Address &other) const override;

  const std::vector<AddressComponentType> &get_address() const;

  static void AssignAddress(uint32_t octree_factor, uint32_t octree_depth,
                            Node &node);

 private:  // TODO: move up
  std::vector<AddressComponentType> data_;
};

template <>
class AddressIterator<SarpAddress> {
 public:
  // Generate empty addresses => empty vector
  // Use SarpAddress::AssignAddreses to assign proper addreses to nodes.
  std::unique_ptr<SarpAddress> GenerateAddress(Position) {
    return std::make_unique<SarpAddress>(
        std::vector<SarpAddress::AddressComponentType>());
  }
};

}  // namespace simulation

#endif  // SARP_SARP_ADDRESS_H_
