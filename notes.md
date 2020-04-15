# TODO

## General

* Check that functions parameters passed as reference are always const (GStyle) otherwise pass it as pointer.
	* On the same note output value should not be a reference, use pointer
	instead.

* Add `final` modifier to classes when possible.

* Simulation options should be passed at runtime, probably use some JSON.

* Add doxygen comments.

* Remove `unordered_map` where possible.

* Remove `unique_ptr` to speed it up (dependent on the profiling).

* Make immediate event creation without constant (new ctor?).

* Use structural bindings when returned a pair.

* Add event priority, pure time comparison is not sufficient.

* ProtocolPacket redesign??

* Consider removing Time start and end from event generators predecessor.


## Simplify network creation
Store network nodes in `std::vector<Node>` of fixed size instead of `std::unique_ptr`.
Consider passing routing as a template parameter.


## Routing updates
Do not create a copy of routing table to each new packet.
Assume that delivery to neighbors would be sooner than time it takes to do an update period so create one instance of routing table which is propagated to neighbors and let protocol packets have reference to it.

TODOs:
- think through


## Statistics log and plot
Make statistics plotable with periodic log time.

Aims:
- create periodic log statistics event which does write to a stream in some csv format so that it's plotable.

Notes:
- also try to log positions so that we can track it
- create some reasonable `.csv` format

TODOs:
- `LogStatsEvent`
- `LogStatsEventGenerator`


## Interface removal - DONE
Delegate responsibilities of interfaces to Nodes.
Main reason is simpler node neighbor listing.


## Movement and positioning - DONE
Subdivide space to rectangular prisms / cubes.

Aims:
- simplify `Network::UpdateInterfaces`

Notes:
- cube side should be based on connection range
- `MoveGenerator` can plan node position in cube i.e. modify `MoveEvent`
- `Network` should store position of nodes in `std::set<cube_id, node_id>`

TODOs:
- `MoveGenerator`
- `MoveEvent`
- `Position`
- `Node::IsConnectedTo`
- finally also `Network::UpdateInterafaces`


