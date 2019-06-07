//
// address.h
//

#ifndef SARP_SIMULATION_STRUCTURE_ADDRESS_H_
#define SARP_SIMULATION_STRUCTURE_ADDRESS_H_

#include <string>
#include <memory>
#include <cstdint>

namespace simulation {

class Address {
 public:
   virtual ~Address() = 0;
   virtual std::unique_ptr<Address> Clone() const = 0;
   virtual operator std::string() const = 0;
   virtual bool operator==(const Address &) const = 0;
   virtual int get_length() const = 0;
};

class SimpleAddress final : public Address {
 public:
  SimpleAddress(uint32_t addr);
  SimpleAddress(const SimpleAddress& other);
  ~SimpleAddress() override = default;

  std::unique_ptr<Address> Clone() const override;
  operator std::string() const override;
  bool operator==(const Address &other) const override;
  bool operator<(const SimpleAddress &other) const;
  int get_length() const override;

  uint32_t get_address() const;

 private:
  uint32_t addr_;
};

}  // namespace simulation

#endif  // SARP_SIMULATION_STRUCTURE_ADDRESS