#pragma once

#include <Component/Composable.hh>
#include <Component/Types.hh>
#include <utility/bit.hh>
#include <utility/tuple/toVectorVariant.hh>
#include <cassert>
#include <span>
#include <vector>

class Template;
class World;

// Archetypes store components of entities that are the same, i.e. have exactly the same components.
class Archetype : public ComponentComposable
{
public:

	Archetype(ComponentComposition, World*);

	// Currently Archetypes shall not be copied/moved to prevent pointer invalidation.
	Archetype(const Archetype&) = delete;
	Archetype(Archetype&&) = delete;
	void operator=(const Archetype&) = delete;

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

	void instantiate(const Template&);
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
