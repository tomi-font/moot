#pragma once

#include <Component/Composition.hh>

class EntityId
{
public:

	EntityId() : index(0) {}

	// Constructs an entity's ID using its composition and index.
	EntityId(ComponentComposition comp, unsigned index) : comp(comp), index(index) {}

	const ComponentComposition comp;
	const unsigned index;
};
