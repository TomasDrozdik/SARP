//
// connection.cc
//

#include "structure/connection.h"

namespace simulation {

Connection::Connection(const Node &node, const Position position)
    : position(position), node_(node)  { }

Connection::~Connection() { }

}  // namespace simulation
