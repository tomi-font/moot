#pragma once

#include <moot/Entity/Query.hh>

class EntityQuerier
{
public:

	void match(Archetype*);

	void entityAddedCallback(const Entity&) const;
	void entityRemovedCallback(const Entity&) const;

protected:

	std::vector<EntityQuery> m_queries;
};
