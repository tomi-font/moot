#pragma once

#include <Component/Composition.hh>

class EntityId
{
public:

	EntityId() : m_value(0) {}

	// Constructs an entity's ID using its composition and index.
	EntityId(ComponentComposition, unsigned index);

	ComponentComposition comp() const;
	unsigned index() const;

	using Id = unsigned;
	operator Id() const { return m_value; }

private:

	const Id m_value;
};
