# TODO

* Consider merging `Time` and `is_absolute_time` into some  structure.

* Convert from CMake to Make.
	* Add separate `include` folder.
	* Implicit rules for binaries -> move to `obj`.
	* Explicitly write required binaries to specific programs.

* Consider simpler move and position model.
	* Something like manhatan style in 3D.

* Check that fucntions parameters passed as reference are always const (GStyle)
	otherwise pass it as pointer.
	* On the same note output value should not be a reference, use pointer
	instead.

* Add `final` modifier to classes when possible.

* Use enum for `is_routing_packet`.


