//
// types.h
//

#ifndef SARP_STRUCTURE_TYPES_H_
#define SARP_STRUCTURE_TYPES_H_

#include <cassert>
#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

namespace simulation {

using Time = std::size_t;

using NodeID = std::size_t;

using AddressComponent = unsigned char;

using Address = std::vector<AddressComponent>;

template <typename T>
using range = std::pair<T, T>;

enum class TimeType {
  ABSOLUTE,
  RELATIVE,
};

enum class RoutingType { DISTANCE_VECTOR, SARP };

enum class PacketType { ROUTING, DATA };

std::ostream &operator<<(std::ostream &os, const Address &addr);

std::ostream &operator<<(std::ostream &os, const RoutingType &r);

template <typename T>
std::ostream &operator<<(std::ostream &os, const range<T> &r) {
  return os << r.first << ',' << r.second;
}

}  // namespace simulation

#endif  // SARP_STRUCTURE_TYPES_H_
