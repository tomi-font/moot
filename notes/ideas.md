- Replace manual bitfields with std::bitset.

- Remove duplication of entity parameters (e.g. position, size) among its components.

- Have custom asserts with messages.

- Implement a more elegant way to create entities with all their components.

- depth rendering

	- criteria (top-down, platform)
	- Either sort the entities in place.
	- Or store all the vertices separately (where?).

- Extend ComponentGroup::getAll() to allow specifying several component types and be able to iterate over ~tuples, allowing one element to be an Entity. (See structured bindings.)
