#pragma once

#include <Component/Composition.hh>
#include <Entity/Archetype/iteration.hh>
#include <span>
#include <vector>

// Groups are the way systems express their interest towards different component compositions.
class ComponentGroup
{
public:

	ComponentGroup() {}
	ComponentGroup(ComponentComposition required, ComponentComposition forbidden = ComponentComposition(), bool initializesEntities = false);

	// Appends the given Archetype upon match.
	void match(Archetype*);

	bool matches(ComponentComposition comp) const;
	bool initializesEntities() const { return m_initializesEntities; }

	std::span<Archetype* const> archetypes() const { return m_archs; }

	Archetype* getArchetype(ComponentComposition comp) const;

	ArchetypeIterator<EntityContext> begin() const { return { m_archs.begin() }; }
	ArchetypeIterator<EntityContext> end() const { return { m_archs.end() }; }

	// Allows iterating over all the components (of the given type) matched by this group.
	template<typename C> ArchetypeIterable<C> getAll() const { return { m_archs }; }

private:

	// Matching archetypes.
	std::vector<Archetype*> m_archs;

	// Components whose presence is required.
	ComponentComposition m_required;
	// Components whose presence is forbidden.
	ComponentComposition m_forbidden;

	// Whether entities instantiated in matching archetypes need to be initialized.
	bool m_initializesEntities;
};
