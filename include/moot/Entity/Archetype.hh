#pragma once

#include <moot/Component/Composable.hh>
#include <moot/Component/Types.hh>
#include <moot/Entity/Context.hh>
#include <moot/util/bit.hh>
#include <moot/util/tuple/toVectorVariant.hh>
#include <cassert>
#include <span>
#include <vector>
#include <SFML/System/NonCopyable.hpp>

class Prototype;
class World;

// Archetypes store components of entities that are the same, i.e. have exactly the same components.
class Archetype : public ComponentComposable, sf::NonCopyable
{
public:

	Archetype(ComponentComposition, World*);

	World* world() const { return m_world; }

	auto entityCount() const
	{
		assert(m_entityCount == computeEntityCount());
		return m_entityCount;
	}

	// Returns the vector containing the component type requested.
	template<typename C> std::span<C> getAll()
	{
		// Components will always be stored in the ComponentVectorVariant order.
		// The index is how many bits are set before this component type's bit.
		const unsigned index = setBitCount(m_comp.bits() & (CId<C>.bits() - 1));

		return std::get<std::vector<C>>(m_entities[index]);
	}
	// Returns the component of the requested type found at the given idex.
	template<typename C> C* get(unsigned index) { return &getAll<C>()[index]; }

	EntityContext instantiate(const Prototype&);
	// Removes the entity at the given index.
	void remove(unsigned);

private:

	/* The world this Archetype belongs to. */
	World* const m_world;

	unsigned computeEntityCount() const;
	unsigned m_entityCount;

	// Variant containing vectors of every component.
	using ComponentVectorVariant = tupleToVectorVariant<Components>::type;

	// Components of the same type are stored contiguously; one vector for each.
	std::vector<ComponentVectorVariant>	m_entities;
};
