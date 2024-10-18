#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Entity/Archetype/iteration.hh>
#include <span>
#include <vector>

class EntityQuery
{
	using EntityCallback = std::function<void(const Entity&)>;

public:

	EntityQuery() = default;
	EntityQuery(ComponentComposition required, ComponentComposition forbidden = {}, EntityCallback entityAddedCallback = {}, EntityCallback entityRemovedCallback = {}) : EntityQuery({required}, forbidden, entityAddedCallback, entityRemovedCallback) {}
	EntityQuery(std::initializer_list<ComponentComposition> required, ComponentComposition forbidden = {}, EntityCallback entityAddedCallback = {}, EntityCallback entityRemovedCallback = {});

	void match(Archetype*);
	std::span<Archetype* const> matchedArchetypes() const { return m_matchedArchs; }

	void entityAddedCallback(const Entity&) const;
	void entityRemovedCallback(const Entity&) const;

	ArchetypeIterator<EntityContext> begin() const { return { m_matchedArchs.begin() }; }
	ArchetypeIterator<EntityContext> end() const { return { m_matchedArchs.end() }; }

	// Allows iterating over all the components of the given type belonging to entities matched by this query.
	template<typename C> ArchetypeIterable<C> getAll() const { return { m_matchedArchs }; }

private:

	bool matches(ComponentComposition) const;

	std::vector<Archetype*> m_matchedArchs;

	// Components whose presence is required. Every different composition is an or; any one of them matching will fulfill the required criteria.
	std::vector<ComponentComposition> m_required;
	// Components whose presence is forbidden.
	ComponentComposition m_forbidden;

	EntityCallback m_entityAddedCallback;
	EntityCallback m_entityRemovedCallback;
};
