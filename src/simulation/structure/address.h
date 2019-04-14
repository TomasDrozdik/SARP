//
// address.h
//

#ifndef SARP_SIMULATION_STRUCTURE_ADDRESS_H_
#define SARP_SIMULATION_STRUCTURE_ADDRESS_H_

#include <cstdint>

namespace simulation {

class Address {
 public:
   virtual ~Address() = 0;
   virtual bool operator==(const Address &) = 0;
   virtual bool operator<(const Address &) = 0;
   virtual int get_length() const = 0;
};

class SimpleAddress : Address {
 public:
  SimpleAddress(uint32_t addr);
  ~SimpleAddress() override;

  bool operator==(const Address &other) override;
  bool operator<(const Address &other) override;
  int get_length() const override;

  uint32_t get_address() const;

 private:
  uint32_t addr_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ADDRESS
