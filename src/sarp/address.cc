//
// sarp_address.cc
//

#include "sarp/address.h"

#include <cmath>

namespace simulation {

SarpAddress::SarpAddress(const std::vector<unsigned char> &data) :
    data_(data) { }

SarpAddress::SarpAddress(std::vector<unsigned char> &&data) :
    data_(data) { }

void SarpAddress::AssignAddresses(Network &network) {
  const uint32_t octree_factor = sizeof (SarpAddress::AddressComponentType) * 8;

  // Count min distance between nodes => how deep should the octree be.
  Position::Unit min_node_distance = 0;
  auto &nodes = network.get_nodes();
  for (std::size_t i = 0; i < nodes.size(); ++i) {
    for (std::size_t j = i + 1; j < nodes.size(); ++j) {
      auto distance = Position::Distance(nodes[i]->get_connection().position,
          nodes[j]->get_connection().position);
      if (min_node_distance > distance) {
        min_node_distance = distance;
      }
    }
  }

  // According to min distance between nodes count the depth of the octree =>
  // length of addresses / depth of address space tree.
  std::size_t depth = 1;
  // min_cube_* is the size of *side of the smallest cube according to the
  // given depth of octree.
  auto min_cube_x = (Position::max->x - Position::min->x) / octree_factor;
  auto min_cube_y = (Position::max->y - Position::min->y) / octree_factor;
  auto min_cube_z = (Position::max->z - Position::min->z) / octree_factor;
  //typeof (min_node_distance) min_cube_diagonal =
  auto min_cube_diagonal =
      std::sqrt(min_cube_x * min_cube_x + min_cube_y * min_cube_y +
          min_cube_z * min_cube_z);
  for (++depth; min_cube_diagonal < min_node_distance; ++depth) {
    min_cube_x /= octree_factor;
    min_cube_y /= octree_factor;
    min_cube_z /= octree_factor;
    min_cube_diagonal = std::sqrt(min_cube_x * min_cube_x +
        min_cube_y * min_cube_y + min_cube_z * min_cube_z);
  }

  // Now that we know the depth assign address to each node.
  for (auto &node : nodes) {
    AssignAddress(octree_factor, depth, *node);
  }
}

std::unique_ptr<Address> SarpAddress::Clone() const {
  return std::make_unique<SarpAddress>(data_);
}

std::size_t SarpAddress::Hash() const {
  std::hash<AddressComponentType> hasher;
  std::size_t hash_value = hasher(data_[0]);

  for (std::size_t i = 1; i < data_.size(); ++i) {
    // https://stackoverflow.com/a/2595226
    hash_value ^= hasher(data_[i]) + 0x9e3779b9 +
        (hash_value<<6) + (hash_value>>2);
  }
  return hash_value;
}

std::ostream &SarpAddress::Print(std::ostream &os) const {
  std::size_t i;
  if (data_.size() == 0) {
    return os << "empty";
  }

  for (i = 0; i < data_.size() - 1; ++i) {
    os << static_cast<unsigned>(data_[i]) << '.';
  }
  return os << static_cast<unsigned>(data_[i]);
}

bool SarpAddress::operator==(const Address &other) const {
  auto &other_data = dynamic_cast<const SarpAddress &>(other).data_;
  if (data_.size() != other_data.size()) {
    return false;
  }
  for (std::size_t i = 0; i < data_.size(); ++i) {
    if (data_[i] != other_data[i]) {
      return false;
    }
  }
  return true;
}

bool SarpAddress::operator<(const Address &other) const {
  auto &other_data = dynamic_cast<const SarpAddress &>(other).data_;
  for (std::size_t i = 0; i < data_.size() && i < other_data.size(); ++i) {
    if (data_[i] != other_data[i]) {
      return data_[i] < other_data[i];
    }
  }
  return false;
}

const std::vector<SarpAddress::AddressComponentType> &SarpAddress::get_address()
    const {
  return data_;
}

Position::Unit ProcessOneComponent(uint32_t octree_factor, double &max_cube_side,
    int &relative_pos);

void SarpAddress::AssignAddress(uint32_t octree_factor, uint32_t octree_depth,
    Node &node) {
  std::vector<AddressComponentType> address;
  Position pos = node.get_connection().position;
  double max_x = Position::max->x;
  double max_y = Position::max->y;
  double max_z = Position::max->z;
  for (uint32_t i = 1; i <= octree_depth; ++i) {
    address.push_back(ProcessOneComponent(octree_factor, max_x, pos.x)
        + ProcessOneComponent(octree_factor, max_y, pos.y) * octree_factor
        + ProcessOneComponent(octree_factor, max_z, pos.z)
            * octree_factor * octree_factor);
  }
  node.add_address(std::make_unique<SarpAddress>(address));
}

Position::Unit ProcessOneComponent(uint32_t octree_factor, double &max_cube_side,
    int &relative_pos) {
  Position::Unit addr_component = relative_pos / (max_cube_side / octree_factor);
  if (addr_component == octree_factor) {
    addr_component = octree_factor - 1;
  }
  max_cube_side /= octree_factor;
  relative_pos -= addr_component * max_cube_side;
  return addr_component;
}

}  // namespace simulation
