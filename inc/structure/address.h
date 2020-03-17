//
// address.h
//

#ifndef SARP_STRUCTURE_ADDRESS_H_
#define SARP_STRUCTURE_ADDRESS_H_

#include <cstddef>
#include <ostream>
#include <vector>

namespace simulation {

using Address = std::vector<unsigned char>;

std::ostream &operator<<(std::ostream &os, const Address &addr);

}  // namespace simulation

#endif  // SARP_STRUCTURE_ADDRESS
