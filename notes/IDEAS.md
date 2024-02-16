- when adding an entity, process some kind of sorting depending on components values (i.e. for rendering: depth)
	potential issues: components are constructed only when appended to the archetype, different sortings criterias might interfer and make it impossible

- messaging an entity: a message would have a t_EntityComp to specify which components are of interest, then simply masking it to see if anything matches

- depth rendering: store all vertices at a same depth contiguously, call draw() with a sf::Transform to avoid moving all vertices all the time

- arrays performance: avoiding reallocations as much as possible (currently, vectors are let to default behaviour)

- Separate the event listeners and triggers.

- Systems' storing
	- Fixed indices may be too restrictive e.g. when system dependencies get more complex.

- Duplication of entity parameters (e.g. position, size) among its components

- event system
	- Having every listener store a reference to the manager may not be the most elegant.

- Have a size component that other components make use of.

- Extend ComponentGroup::getAll() to allow specifying component types and be able to iterate over ~tuples (allowing with or without one element being EntityHandle?). (See structured bindings.) Not sure of the feasability.
