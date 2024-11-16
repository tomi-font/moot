#pragma once

#include <moot/Entity/Query.hh>

class EntityQuerier
{
public:

	void match(Archetype*);

	void onEntityAdded(const Entity&) const;
	void onEntityRemoved(const Entity&) const;
	
	void onChangedEntityRemoved(const Entity& oldEntity, ComponentComposition newComp) const;
	void onChangedEntityAdded(const Entity& newEntity, ComponentComposition oldComp) const;

protected:

	std::vector<EntityQuery> m_queries;
};
