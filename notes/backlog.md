- Review how the ComponentGroups are created/referred to.

- Update SFML to 2.6.

- Switch to using C++ modules.

- depth rendering
	- criteria (top-down, platform)
	- Either sort the entities in place.
	- Or store all the vertices separately (where?).

- Scale things according to real world values.

- Implement own Event struct for input events.

- Logging with various verbosity levels/topics.
- Custom asserts with messages.

- Implement a ComponentGroup::getAllContiguous().
- Extend ComponentGroup::getAll() to allow specifying several component types and be able to iterate over ~tuples, allowing one element to be an Entity. (See structured bindings.)

- Process all the collisions in the order they happen.
- Foolproof the collisions when the move is bigger than the object.

- Remove duplication of entity parameters (e.g. position, size) among its components?
