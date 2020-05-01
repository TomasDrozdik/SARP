//
// record_redundancy.main.cc
//
//
//#include <iomanip>
//#include <iostream>
//
//#include "sarp/routing.h"
//
// namespace simulation {
//
// std::ostream &operator<<(std::ostream &os, const Cost &r) {
//  return os << "Cost{" << r.mean << ',' << r.sd << ',' << r.group_size << '}';
//}
//
// class CostTests {
// public:
//  using Cost = Cost;
//
//  static std::size_t FindRedundancyTreshold(Cost default_record) {
//    std::size_t hop_count = 1;
//    Cost record(default_record);
//    // record.AddCost(default_record);  // Emulate 1 hop distance node.
//
//    while (default_record.AreSimilar(record)) {
//      std::cerr << default_record << ' ' << record << ' '
//                << Cost::ZTest(default_record, record) << '\n';
//      ++hop_count;
//      // record.AddCost(default_record);
//    }
//    return hop_count;
//  }
//
//  static void Main() {
//    for (double sd = 0.1; sd < 1; sd += 0.1) {
//      SarpRouting::Cost r{.mean = 1, .sd = sd, .group_size = 1};
//      auto treshold = FindRedundancyTreshold(r);
//      std::cout << r << ' ' << sd << ' ' << treshold << '\n';
//    }
//  }
//};
//
//}  // namespace simulation
//
// int main() {
//  simulation::CostTests::Main();
//  return 0;
//}
