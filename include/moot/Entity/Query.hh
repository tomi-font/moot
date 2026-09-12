#pragma once

#include <moot/Component/Collection/Iterator.hh>
#include <functional>
#include <vector>

class EntityQuery
{
	using Callback = std::function<void(const EntityPointer&)>;
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
	EntityQuery(Parameters&&);

	bool matches(ComponentComposition) const;
	void match(ComponentCollection*);

	auto& matchingCollections() const { return m_matchingCollections; }

	auto& onEntityAdded() const { return m_params.onEntityAdded; }
	auto& onEntityRemoved() const { return m_params.onEntityRemoved; }

	ComponentCollectionIterator<EntityPointer> begin() const { return {m_matchingCollections.begin()}; }
	ComponentCollectionIterator<EntityPointer> end() const { return {m_matchingCollections.end()}; }

	template<typename ...Cs> ComponentCollectionIterable<Cs...> getAll() const { return {m_matchingCollections}; }

	unsigned getEntityCount() const;
	// The one entity matching the query, for queries meant to match a single entity.
	EntityPointer getSingleEntity() const;

private:

	std::vector<ComponentCollection*> m_matchingCollections;

	Parameters m_params;
};
