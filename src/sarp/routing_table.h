//
// routing_table.h
//

#ifndef SARP_SARP_ROUTING_TABLE_H_
#define SARP_SARP_ROUTING_TABLE_H_

#include "../structure/interface.h"

namespace simulation {

class SarpRoutingTable {
 public:

 private:
	struct Record {
		Record(Interface *via_interface, double cost_mean,
				double cost_standard_deviation, std::size_t group_size);

		void MergeWith(const Record &other);

		Interface *via_interface;
		double cost_mean;
		double cost_standard_deviation;
		double group_size;  // In log scale with base 1.1
	};

};

}  // namespace simulation

#endif  // SARP_SARP_ROUTING_TABLE_H_