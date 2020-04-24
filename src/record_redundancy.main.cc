//
// record_redundancy.main.cc
//

#include <iostream> 
#include <iomanip> 

#include "sarp/routing.h"

namespace simulation {

std::ostream &operator<<(std::ostream &os, const SarpRouting::Record &r) {
  return os << "Record{" << r.cost_mean << ',' << r.cost_sd << ',' << r.group_size << '}';
}

class RecordTests {
 public:
  using Record = SarpRouting::Record;

  static std::size_t FindRedundancyTreshold(Record default_record) {
     std::size_t hop_count = 1;
     Record record(default_record);
     record.AddRecord(default_record);  // Emulate 1 hop distance node.

     while (!default_record.AreSimilar(record)) {
       std::cerr << default_record << ' ' << record << ' ' <<  Record::ZTest(default_record, record) << '\n';
       if (++hop_count >= 10) break;
       record.AddRecord(default_record);
     }
     return hop_count;
  }

  static void Main() {
    for (double sd = 0.21; sd < 0.25; sd += 0.01) {
      SarpRouting::Record r{.cost_mean = 1, .cost_sd = sd, .group_size = 1};
      auto treshold = FindRedundancyTreshold(r);
      std::cout << r << ' ' << sd << ' ' << treshold << '\n';
    }
  }
};

}  // namespace simulation

int main() {
  simulation::RecordTests::Main();
  return 0;
}

