//
// address.h
//

#ifndef SARP_STRUCTURE_ADDRESS_H_
#define SARP_STRUCTURE_ADDRESS_H_

#include <memory>
#include <ostream>
#include <string>
#include <cstdint>

namespace simulation {

class Address {
 friend std::ostream &operator<<(std::ostream &os, const Address &addr);
 public:
  virtual ~Address() = 0;
  virtual std::unique_ptr<Address> Clone() const = 0;
  virtual std::size_t Hash() const = 0;
  virtual std::ostream &Print(std::ostream &os) const = 0;
  virtual bool operator==(const Address &) const = 0;
  virtual bool operator<(const Address &other) const = 0;
};

struct AddressHash {
  std::size_t operator()(const std::unique_ptr<Address> &addr) const;
};

struct AddressComparer {
  bool operator()(const std::unique_ptr<Address> &a1,
      const std::unique_ptr<Address> &a2) const;
};

class SimpleAddress final : public Address {
 public:
  SimpleAddress(uint32_t addr);
  SimpleAddress(const SimpleAddress& other);
  ~SimpleAddress() override = default;

  std::unique_ptr<Address> Clone() const override;
  std::size_t Hash() const override;
  std::ostream &Print(std::ostream &os) const override;
  bool operator==(const Address &other) const override;
  bool operator<(const Address &other) const override;

 private:
  uint32_t addr_;
};

}  // namespace simulation

#endif  // SARP_STRUCTURE_ADDRESS
