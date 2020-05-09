//
// record_redundancy.main.cc
//
//

#include <iomanip>
#include <iostream>

#include "sarp/cost.h"

using namespace simulation;

std::ostream &operator<<(std::ostream &os, const Cost &r) {
  return os << "Cost{" << r.mean << ',' << r.sd << ',' << r.group_size << '}';
}

std::size_t FindRedundancyTreshold(Cost default_cost) {
  std::size_t hop_count = 1;
  Cost c = default_cost;
  Cost next_c = Cost::AddCosts(c, default_cost);
  while (!Cost::AreSimilar(c, next_c, 1.96)) {
    // std::cerr << default_cost << ' ' << c << ' ' << next_c << ' '
    //          << Cost::ZTest(next_c, c) << '\n';
    ++hop_count;
    c = Cost::AddCosts(c, default_cost);
    next_c = Cost::AddCosts(next_c, default_cost);
  }
  return hop_count;
}

int main() {
  // Print csv header
  std::cout << "sd, hop\n";
  std::size_t last_treshold = 0;
  for (double sd = 0.001; sd < 0.3; sd += 0.001) {
    Cost default_cost{.mean = 1, .sd = sd, .group_size = 1};
    auto treshold = FindRedundancyTreshold(default_cost);
    if (treshold != last_treshold) {
      std::cout << sd << ", " << treshold << '\n';
      last_treshold = treshold;
    }
  }

  // Cost default_c = {.mean = 1, .sd = 1, .group_size = 1};
  // Cost c = {.mean = 100, .sd = 100, .group_size = 1};
  // Cost next_c = Cost::AddCosts(c, default_c);
  // std::cout << c << ' ' << next_c << ' ' << Cost::ZTest(next_c, c) << ' '
  //  << AreSimilar(c, next_c, 1.96) << '\n';
  return 0;
}
