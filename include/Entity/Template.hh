#pragma once

#include <Component/Composable.hh>
#include <Component/Variant.hh>
#include <set>

class Template : public ComponentComposable
{
public:

	Template() = default;

	// Adds a (not already present) component.
	void add(ComponentVariant&&);

	// Removes a (present) component.
	void remove(ComponentId);

private:

	// A set allows easily inserting components in their sorting order without knowing in advance the final composition.
	std::set<ComponentVariant> m_components;

	friend class Archetype;
};
