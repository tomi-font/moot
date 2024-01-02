#pragma once

#include <Component/Composition.hh>
#include <span>
#include <vector>

class Archetype;

// Groups are the way systems express their interest towards different component compositions.
class ComponentGroup
{
public:

	ComponentGroup() {}
	ComponentGroup(ComponentComposition required, ComponentComposition forbidden = ComponentComposition()) : m_required(required), m_forbidden(forbidden) {}

	// Appends the given Archetype upon match.
	void match(Archetype*);

	std::span<Archetype*> archs() { return m_archs; }

private:

	bool matches(ComponentComposition comp) const;

	// Matching archetypes.
	std::vector<Archetype*> m_archs;

	// Components whose presence is required.
	ComponentComposition m_required;
	// Components whose presence is forbidden.
	ComponentComposition m_forbidden;
};
