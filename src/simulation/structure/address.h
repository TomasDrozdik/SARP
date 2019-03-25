//
// address.h
//

#ifndef SARP_SIMULATION_STRUCTURE_ADDRESS_H_
#define SARP_SIMULATION_STRUCTURE_ADDRESS_H_

namespace simulation {

class Address {
 public:
   virtual void Set(const Address &) = 0;  // ?? :D
   virtual bool operator==(const Address &) = 0;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ADDRESS