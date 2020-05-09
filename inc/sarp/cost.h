//
// cost.h
//

#ifndef SARP_SARP_COST_H_
#define SARP_SARP_COST_H_

#include <cmath>

namespace simulation {

// Cost which belongs to a route to an address.
// It is represented by a normal distribution with hop count metrics as a
// cost.
// Additionaly record stores the size of the group on the route.
struct Cost {
  static Cost AddCosts(const Cost &c1, const Cost &c2) {
    // group size remains the same
    return {.mean = c1.mean + c2.mean, .var = c1.var + c2.var};
  }

  // Function which determines which cost info routing will keep when merging
  // two records with same address but different costs.
  bool PreferTo(const Cost &other) const { return mean < other.mean; }

  static double ZScore(const Cost &r1, const Cost &r2) {
    // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
    double sd1 = std::sqrt(r1.var);
    double sd2 = std::sqrt(r2.var);
    return (r1.mean - r2.mean) / std::sqrt(sd1 * sd1 + sd2 * sd2);
  }

  double mean;
  double var;
  double group_size;  // TODO: In log scale with base 1.1.
};

bool operator==(const Cost &c1, const Cost &c2);

bool operator!=(const Cost &c1, const Cost &c2);

}  // namespace simulation

#endif  // SARP_SARP_COST_H_
