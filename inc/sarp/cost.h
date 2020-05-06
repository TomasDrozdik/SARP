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
    return {.mean = c1.mean + c2.mean, .sd = c1.sd + c2.sd};
  }

  // Function which determines which cost info routing will keep when merging
  // two records with same address but different costs.
  bool PreferTo(const Cost &other) const { return mean < other.mean; }

  static double ZTest(const Cost &r1, const Cost &r2) {
    // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
    double z_score =
        (r1.mean - r2.mean) / std::sqrt(r1.sd * r1.sd + r2.sd * r2.sd);
    return z_score;
  }

  // Declare whether the other normal distribution is 'the same' => redundant
  // to this normal distribution according to Z-test:
  // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
  bool AreSimilar(const Cost &other, double quantile_treshold) const {
    auto z_score = Cost::ZTest(*this, other);
    // Now compare with quantile with parameters[https://planetcalc.com/4987]:
    //  Probability 0.975
    //  Variance    1
    //  Mean        0
    if (z_score == 0) {
      // TODO: rethink this.
      // They are same so don't.
      return false;
    }
    return std::abs(z_score) < quantile_treshold;
  }

  double mean;
  double sd;
  double group_size;  // TODO: In log scale with base 1.1.
};

bool operator==(const Cost &c1, const Cost &c2);

bool operator!=(const Cost &c1, const Cost &c2);

}  // namespace simulation

#endif  // SARP_SARP_COST_H_
