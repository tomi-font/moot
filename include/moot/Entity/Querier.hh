#pragma once

#include <moot/Entity/Query.hh>

class EntityQuerier
{
public:

	void match(ComponentCollection*);

	void onEntityAdded(const EntityPointer&) const;
	void onEntityRemoved(const EntityPointer&) const;
	
	void onChangedEntityRemoved(const EntityPointer& oldEntity, ComponentComposition newComp) const;
	void onChangedEntityAdded(const EntityPointer& newEntity, ComponentComposition oldComp) const;

protected:

	std::vector<EntityQuery> m_queries;
};
