#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Entity/Archetype/iteration.hh>
#include <span>
#include <vector>

class EntityQuery
{
	using Callback = std::function<void(const Entity&)>;
	struct Parameters
	{
		// Components whose presence is required. Every different composition is an or; any one of them matching will fulfill the `required` criteria.
		std::vector<ComponentComposition> required;
		// Components whose presence is forbidden.
		ComponentComposition forbidden;
		Callback onEntityAdded;
		Callback onEntityRemoved;
	};

public:

	EntityQuery() = default;
	EntityQuery(Parameters&& args);

	bool matches(ComponentComposition) const;
	void match(Archetype*);

	std::span<Archetype* const> matchedArchetypes() const { return m_matchedArchs; }

	auto& onEntityAdded() const { return m_params.onEntityAdded; }
	auto& onEntityRemoved() const { return m_params.onEntityRemoved; }

	ArchetypeIterator<EntityPointer> begin() const { return { m_matchedArchs.begin() }; }
	ArchetypeIterator<EntityPointer> end() const { return { m_matchedArchs.end() }; }

	// Allows iterating over all the components of the given type belonging to entities matched by this query.
	template<typename C> ArchetypeIterable<C> getAll() const { return { m_matchedArchs }; }

private:

	std::vector<Archetype*> m_matchedArchs;

	Parameters m_params;
};
