#pragma once

#include <Component/Composable.hh>
#include <Component/Types.hh>
#include <utility/bit.hh>
#include <utility/tuple/toVectorVariant.hh>
#include <cassert>
#include <span>
#include <vector>

class Template;

// Archetypes store components of entities that are the same, i.e. have exactly the same components.
class Archetype : public ComponentComposable
{
public:

	Archetype(ComponentComposition);

	// Currently Archetypes shall not be copied/moved to prevent pointer invalidation.
	Archetype(const Archetype&) = delete;
	void operator=(const Archetype&) = delete;

	auto entityCount() const
	{
		assert(m_entityCount == computeEntityCount());
		return m_entityCount;
	}

	// Returns the vector containing the component type requested.
	template<typename T>
	std::span<T> getAll() noexcept
	{
		// Sanity check that the requested component type is present.
		assert(m_comp.has(CId<T>));

		// Components will always be stored in the ComponentVectorVariant order.
		// The index is how many bits are set before this component type's bit.
		const unsigned index = setBitCount(m_comp.bits() & (CId<T>.bits() - 1));

		return std::get<std::vector<T>>(m_entities[index]);
	}

	void instantiate(const Template&);

private:

	unsigned computeEntityCount() const;

	unsigned m_entityCount;

	// Variant containing vectors of every component.
	using ComponentVectorVariant = tupleToVectorVariant<Components>::type;

	// Components of the same type are stored contiguously; one vector for each.
	std::vector<ComponentVectorVariant>	m_entities;
};
