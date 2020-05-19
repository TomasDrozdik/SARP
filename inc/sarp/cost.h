//
// cost.h
//

#ifndef SARP_SARP_COST_H_
#define SARP_SARP_COST_H_

#include <cmath>
#include <vector>
#include <numeric>
#include <ostream>

namespace simulation {

inline static double pow2(double x) { return x * x; }

// Cost which belongs to a route to an address.
// It is represented by a normal distribution with hop count metrics as a
// cost.
// Additionaly record stores the size of the group on the route.
//
struct Cost {
  friend std::ostream &operator<<(std::ostream &os, const Cost &cost);
  friend bool operator==(const Cost &lhs, const Cost &rhs);
  friend bool operator!=(const Cost &lhs, const Cost &rhs);
 public:
  Cost() = default;

  Cost(double mean, double variance) : mean_(mean), variance_(variance) {}

  Cost(const std::vector<Cost> &costs) {
    if (costs.size() == 1) {
      mean_ = costs[0].mean_;
      variance_ = costs[0].variance_;
      return;
    }
    auto SumMeans = [](double sum_accumulator, const Cost &c) {
                      return sum_accumulator + c.mean_;
                    };
    double sum_means = std::accumulate(costs.cbegin(), costs.cend(), 0.0, SumMeans);
    mean_ = sum_means / costs.size();

    auto ComputeVarianceSum = [this](double sum_accumulator, const Cost &c) {
                                return sum_accumulator + pow2(c.mean_ -  mean_)
                                                       + pow2(c.variance_);
                              };
    double variance_sum = std::accumulate(costs.cbegin(), costs.cend(),
                                           0.0, ComputeVarianceSum);
    variance_ = variance_sum / costs.size();
  }

  static Cost AddCosts(const Cost &c1, const Cost &c2) {
    return Cost(c1.mean_ + c2.mean_, c1.variance_ + c2.variance_);
  }

  // Function which determines which cost info routing will keep when merging
  // two records with same address but different costs.
  bool PreferTo(const Cost &other) const { return mean_ < other.mean_; }

  double ZScore(const Cost &other) const {
    // [http://homework.uoregon.edu/pub/class/es202/ztest.html]
    double this_sd = StandardDeviation();
    double other_sd = other.StandardDeviation();
    return (other.Mean() - Mean()) /
        std::sqrt(this_sd * this_sd + other_sd * other_sd);
  }

  double Mean() const { return mean_; }

  double Variance() const { return variance_; }

  double StandardDeviation() const { return std::sqrt(variance_); }

 private:
  double mean_;
  double variance_;
};

bool operator==(const Cost &lhs, const Cost &rhs);

bool operator!=(const Cost &lhs, const Cost &rhs);

std::ostream &operator<<(std::ostream &os, const Cost &cost);

}  // namespace simulation

#endif  // SARP_SARP_COST_H_
