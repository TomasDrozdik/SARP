//
// sarp_address.h
//

#ifndef SARP_SARP_ADDRESS_H_
#define SARP_SARP_ADDRESS_H_

#include "../structure/address.h"

#include <vector>

namespace simulation {

class SarpAddress : public Address {
 public:
  using AddressComponentType = unsigned char;

	SarpAddress(std::vector<unsigned char> data);
	SarpAddress(const std::vector<unsigned char> &data);
	SarpAddress(std::vector<unsigned char> &&data);

	~SarpAddress() override = default;

	std::unique_ptr<Address> Clone() const override;

	std::size_t Hash() const override;

	std::ostream &Print(std::ostream &os) const override;

	bool operator==(const Address &other) const override;

 private:
  std::vector<AddressComponentType> data_;
};

}  // namespace simulation

#endif  // SARP_SARP_ADDRESS_H_