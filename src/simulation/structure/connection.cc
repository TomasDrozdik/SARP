
//
// connection.cc
//

#include "connection.h"

namespace simulation {

Connection::Connection(const Node &node, const Position position)
		: position_(position), node_(node)  { }

Connection::~Connection() { }

}  // namespace simulation