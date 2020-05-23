//
// types.h
//

#ifndef SARP_STRUCTURE_TYPES_H_
#define SARP_STRUCTURE_TYPES_H_

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>
#include <ostream>

namespace simulation {

using Time = std::size_t;

using Address = std::vector<unsigned char>;

template <typename T>
using range = std::pair<T, T>;

enum class TimeType {
  ABSOLUTE,
  RELATIVE,
};

enum class RoutingType {
  DISTANCE_VECTOR,
  SARP
};

enum class PacketType {
  ROUTING,
  DATA
};

std::ostream &operator<<(std::ostream &os, const Address &addr);

std::ostream &operator<<(std::ostream &os, const RoutingType &r);

template <typename T>
std::ostream &operator<<(std::ostream &os, const range<T> &r) {
  return os << r.first << ',' << r.second;
}

}  // namespace simulation

#endif  // SARP_STRUCTURE_TYPES_H_
