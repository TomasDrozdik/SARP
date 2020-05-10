//
// record_redundancy.main.cc
//

#include <iomanip>
#include <iostream>

#include "sarp/cost.h"

using namespace simulation;

std::ostream &operator<<(std::ostream &os, const Cost &r) {
  return os << "Cost{" << r.mean << ',' << r.var << ',' << r.group_size << '}';
}

int main() {
  // Print csv header
  std::cout << "def_cost, cost, hop, zcore\n";
  double var = 0.1;
  const Cost def = {.mean = 1, .var = var, .group_size = 1};
  Cost c = def;
  for (std::size_t hop = 0; hop < 20; ++hop) {
    std::cout << def << ", " << c << ", " <<  hop << ", " << Cost::ZScore(def, c) << '\n';
    c = Cost::AddCosts(c, def);
  }
  return 0;
}
