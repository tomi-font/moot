- Make iterable Entity::children().

- Break down `parsing/` source files.
- Reorganize `utility/`.

- Extend EntityQuery::getAll() to allow specifying several component types and be able to iterate over ~tuples, allowing one element to be an Entity. (See structured bindings.)

- depth rendering
	- criteria (top-down, platform)
	- Either sort the entities in place.
	- Or store all the vertices separately (where?).
	- Also influences pointables.

- Update to SFML 3.0 once it's released (and fix the empty window name workaround made for libc++ 18).

- Process all the collisions in the order they happen.
- Foolproof the collisions when the move is bigger than the object.
