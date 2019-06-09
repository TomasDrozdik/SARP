//
// address.h
//

#ifndef SARP_STRUCTURE_ADDRESS_H_
#define SARP_STRUCTURE_ADDRESS_H_

#include <string>
#include <memory>
#include <cstdint>

namespace simulation {

class Address {
 friend std::ostream &operator<<(std::ostream &os, const Address &addr);
 public:
   virtual ~Address() = 0;
   virtual std::unique_ptr<Address> Clone() const = 0;
   virtual std::size_t Hash() const = 0;
   virtual std::ostream &Print(std::ostream &os) const = 0;
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
  std::size_t Hash() const override;
  std::ostream &Print(std::ostream &os) const override;
  operator std::string() const override;
  bool operator==(const Address &other) const override;
  bool operator<(const SimpleAddress &other) const;
  int get_length() const override;

 private:
  uint32_t addr_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ADDRESS
