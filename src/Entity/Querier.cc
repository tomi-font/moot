#include <moot/Entity/Querier.hh>

void EntityQuerier::match(Archetype* arch)
{
	for (EntityQuery& query : m_queries)
		query.match(arch);
}

void EntityQuerier::entityAddedCallback(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		query.entityAddedCallback(entity);
}

void EntityQuerier::entityRemovedCallback(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		query.entityRemovedCallback(entity);
}
