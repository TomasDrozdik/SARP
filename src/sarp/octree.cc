//
// octree.cc
//

#include "sarp/octree.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "structure/simulation.h"

namespace simulation {

constexpr uint32_t octree_factor = 2;

OctreeAddressingEvent::OctreeAddressingEvent(const Time time,
                                                           TimeType type,
                                                           Network &network)
    : Event(time, type), network_(network) {}

void OctreeAddressingEvent::Execute(Env &env) {
  RecomputeUniqueAddresses(network_,
                           env.parameters.get_general().boundaries.first,
                           env.parameters.get_general().boundaries.second);
}

std::ostream &OctreeAddressingEvent::Print(std::ostream &os) const {
  return os << time_ << ":sarp_address_update:" << '\n';
}

OctreeAddressingEventGenerator::
    OctreeAddressingEventGenerator(range<Time> time, Time period,
                                             Network &network)
    : time_(time),
      period_(period),
      network_(network),
      virtual_time_(time_.first) {}

std::unique_ptr<Event> OctreeAddressingEventGenerator::Next() {
  if (virtual_time_ >= time_.second) {
    return nullptr;
  }
  auto event = std::make_unique<OctreeAddressingEvent>(
      virtual_time_, TimeType::ABSOLUTE, network_);
  virtual_time_ += period_;
  return std::move(event);
}

static double MinNodeDistance(const Network &network) {
  assert(network.get_nodes().size() >= 2);
  double min_node_distance = std::numeric_limits<int>::max();
  auto &nodes = network.get_nodes();
  for (std::size_t i = 0; i < nodes.size(); ++i) {
    for (std::size_t j = i + 1; j < nodes.size(); ++j) {
      auto distance =
          Position::Distance(nodes[i]->get_position(), nodes[j]->get_position());
      min_node_distance = std::min(min_node_distance, distance);
    }
  }
  // If two nodes are very close to each other then the depth of the octree will
  // be too big -> set some granularity with settin minimal min_node_distance to
  // 1 and therefore setting the limit on address length.
  return std::max(min_node_distance, 1.0);
}

static std::size_t CountOctreeDepth(const Network &network,
                                    uint32_t min_distance, Position min_pos,
                                    Position max_pos) {
  std::size_t depth = 0;
  // According to min distance between nodes count the depth of the octree i.e.
  // length of addresses i.e. depth of address space tree.
  // Min distance represents 'accuracy' of the octree - its depth.
  // cube_* is the size of *side of the cube according to the
  // given depth of octree.
  double cube_x = (max_pos.x - min_pos.x) / (double)octree_factor;
  double cube_y = (max_pos.y - min_pos.y) / (double)octree_factor;
  double cube_z = (max_pos.z - min_pos.z) / (double)octree_factor;
  double cube_diagonal =
      std::sqrt(cube_x * cube_x + cube_y * cube_y + cube_z * cube_z);
  for (++depth; cube_diagonal >= min_distance; ++depth) {
    cube_x /= octree_factor;
    cube_y /= octree_factor;
    cube_z /= octree_factor;
    cube_diagonal =
        std::sqrt(cube_x * cube_x + cube_y * cube_y + cube_z * cube_z);
  }
  return depth;
}

static int ProcessOneComponent(double *cube_side, double *relative_pos) {
  int addr_component = *relative_pos / (*cube_side / octree_factor);
  if (addr_component == octree_factor) {
    addr_component = octree_factor - 1;
  }
  *cube_side /= octree_factor;
  *relative_pos -= addr_component * (*cube_side);
  return addr_component;
}

static Address GetAddress(uint32_t octree_depth, Position nodes_position,
                          Position max_pos) {
  Address address;
  double pos_x = nodes_position.x;
  double pos_y = nodes_position.y;
  double pos_z = nodes_position.z;
  double cube_x = max_pos.x;
  double cube_y = max_pos.y;
  double cube_z = max_pos.z;
  for (uint32_t i = 1; i <= octree_depth; ++i) {
    address.push_back(ProcessOneComponent(&cube_x, &pos_x) +
                      ProcessOneComponent(&cube_y, &pos_y) * octree_factor +
                      ProcessOneComponent(&cube_z, &pos_z) * octree_factor *
                          octree_factor);
  }
  return address;
}

void OctreeAddressingEvent::RecomputeUniqueAddresses(Network &network,
                                                            Position min_pos,
                                                            Position max_pos) {
  if (network.get_nodes().size() < 2) {
    return;
  }
  uint32_t min_distance = MinNodeDistance(network);
  std::size_t depth = CountOctreeDepth(network, min_distance, min_pos, max_pos);
  // Now that we know the depth assign address to each node.
  for (auto &node : network.get_nodes()) {
    auto new_address = GetAddress(depth, node->get_position(), max_pos);
    node->AddAddress(new_address);
  }
}

}  // namespace simulation
