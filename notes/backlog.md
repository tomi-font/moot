- Fully migrate to C++ modules.

- depth rendering
	- criteria (top-down, platform)
	- Either sort the entities in place.
	- Or store all the vertices separately (where?).

- Scale things according to real world values.

- Remove duplication of entity parameters (e.g. position, size) among its components?

- Update to SFML 3.0 (and fix the empty window name workaround made for libc++ 18).
- Then update to libc++ 19 for `import std`.

- Implement own Event struct for input events.

- Logging with various verbosity levels/topics.
- Custom asserts with messages.

- Extend EntityQuery::getAll() to allow specifying several component types and be able to iterate over ~tuples, allowing one element to be an Entity. (See structured bindings.)

- Process all the collisions in the order they happen.
- Foolproof the collisions when the move is bigger than the object.
