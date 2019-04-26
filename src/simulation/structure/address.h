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
   virtual int get_length() const = 0;
};

class SimpleAddress : public Address {
 public:
  SimpleAddress(uint32_t addr);
  SimpleAddress(const SimpleAddress& other);
  ~SimpleAddress() override = default;

  bool operator==(const Address &other) override;
  bool operator<(const SimpleAddress &other) const;
  int get_length() const override;

  uint32_t get_address() const;

 private:
  uint32_t addr_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ADDRESS
