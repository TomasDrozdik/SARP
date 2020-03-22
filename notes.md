# TODO

* Consider simpler move and position model.
	* Something like manhatan style in 3D.

* Check that functions parameters passed as reference are always const (GStyle)
	otherwise pass it as pointer.
	* On the same note output value should not be a reference, use pointer
	instead.

* Add `final` modifier to classes when possible.

* Simulation options should be passed at runtime, probably use some JSON.

* Add doxygen comments.

* Remove `unordered_map` where possible.

* Remove `unique_ptr` to speed it up (dependent on the profiling).

* Make immediate event creation without constant (new ctor?).
