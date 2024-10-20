#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Entity/Archetype/iteration.hh>
#include <span>
#include <vector>

class EntityQuery
{
	using EntityCallback = std::function<void(const Entity&)>;
	struct Parameters
	{
		// Components whose presence is required. Every different composition is an or; any one of them matching will fulfill the `required` criteria.
		std::vector<ComponentComposition> required;
		// Components whose presence is forbidden.
		ComponentComposition forbidden;
		EntityCallback entityAddedCallback;
		EntityCallback entityRemovedCallback;
	};

public:

	EntityQuery() = default;
	EntityQuery(Parameters&& args);

	bool matches(ComponentComposition) const;
	void match(Archetype*);

	std::span<Archetype* const> matchedArchetypes() const { return m_matchedArchs; }

	auto& entityAddedCallback() const { return m_params.entityAddedCallback; }
	auto& entityRemovedCallback() const { return m_params.entityRemovedCallback; }

	ArchetypeIterator<EntityContext> begin() const { return { m_matchedArchs.begin() }; }
	ArchetypeIterator<EntityContext> end() const { return { m_matchedArchs.end() }; }

	// Allows iterating over all the components of the given type belonging to entities matched by this query.
	template<typename C> ArchetypeIterable<C> getAll() const { return { m_matchedArchs }; }

private:

	std::vector<Archetype*> m_matchedArchs;

	Parameters m_params;
};
