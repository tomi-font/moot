#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Entity/Archetype/iteration.hh>
#include <span>
#include <vector>

class EntityQuery
{
	using EntityInitFunction = std::function<void(const Entity&)>;

public:

	EntityQuery() = default;
	EntityQuery(ComponentComposition required, ComponentComposition forbidden = {}, EntityInitFunction entityInitFunc = {}) : EntityQuery({required}, forbidden, entityInitFunc) {}
	EntityQuery(std::initializer_list<ComponentComposition> required, ComponentComposition forbidden = {}, EntityInitFunction = {});

	// Appends the given Archetype upon match.
	void match(Archetype*);

	void initializeEntity(const Entity& entity) const;

	std::span<Archetype* const> matchedArchetypes() const { return m_matchedArchs; }

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

	EntityInitFunction m_entityInitFunc;
};
