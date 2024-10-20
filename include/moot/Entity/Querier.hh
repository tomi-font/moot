#pragma once

#include <moot/Entity/Query.hh>

class EntityQuerier
{
public:

	void match(Archetype*);

	void entityAddedCallback(const Entity&) const;
	void entityRemovedCallback(const Entity&) const;
	
	void entityChangedRemovedCallback(const Entity& oldEntity, ComponentComposition newComp) const;
	void entityChangedAddedCallback(const Entity& newEntity, ComponentComposition oldComp) const;

protected:

	std::vector<EntityQuery> m_queries;
};
