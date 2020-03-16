# TODO

* Consider merging `Time` and `is_absolute_time` into some  structure.

* Consider simpler move and position model.
	* Something like manhatan style in 3D.

* Check that functions parameters passed as reference are always const (GStyle)
	otherwise pass it as pointer.
	* On the same note output value should not be a reference, use pointer
	instead.

* Add `final` modifier to classes when possible.

* Use enum for `is_routing_packet`.

* Simulation options should be passed at runtime, probably use some JSON.

* Add doxygen comments.

* Make Address non virtual as vector of bytes.

* Remove `unordered_map` where possible.

* Remove `unique_ptr` to speed it up (dependent on the profiling).

