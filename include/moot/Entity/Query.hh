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

	// For queries meant to match a single entity.
	template<typename ...Cs> decltype(auto) getSingle() const
	{
		return *ComponentCollectionIterator<Cs...>(getSingleIterator());
	}

	unsigned getEntityCount() const;

private:

	std::vector<ComponentCollection*>::const_iterator getSingleIterator() const;

	std::vector<ComponentCollection*> m_matchingCollections;

	Parameters m_params;
};
