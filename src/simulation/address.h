//
// address.h
//

#ifndef SARP_SMULATION_ADDRESS_H_
#define SARP_SMULATION_ADDRESS_H_

namespace simulation {

class Address {
 public:
   virtual void Set(const Address &) = 0;  // ?? :D
   virtual bool operator==(const Address &) = 0;
};

}  // namespace simulation

#endif  // SARP_SMULATION_ADDRESS_H_
