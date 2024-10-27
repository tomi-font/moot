- Move ComponentComposable inheritance away from EntityContext?

- Clean up CView updates.

- Rename Template to something else?

- Remove duplication of entity parameters (e.g. position, size) among its components.

- Extend EntityQuery::getAll() to allow specifying several component types and be able to iterate over ~tuples, allowing one element to be an Entity. (See structured bindings.)

- Break down `parsing/` source files.

- depth rendering
	- criteria (top-down, platform)
	- Either sort the entities in place.
	- Or store all the vertices separately (where?).
	- Also influences pointables.

- Scale things according to real world values.

- Update to SFML 3.0 (and fix the empty window name workaround made for libc++ 18).

- Implement own Event struct for input events.

- Logging with various verbosity levels/topics.
- Custom asserts with messages.

- Process all the collisions in the order they happen.
- Foolproof the collisions when the move is bigger than the object.
