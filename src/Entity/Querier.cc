#include <moot/Entity/Querier.hh>
#include <moot/Entity/Entity.hh>

void EntityQuerier::match(Archetype* arch)
{
	for (EntityQuery& query : m_queries)
		query.match(arch);
}

void EntityQuerier::entityAddedCallback(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.entityAddedCallback() && query.matches(entity.comp()))
			query.entityAddedCallback()(entity);
}

void EntityQuerier::entityRemovedCallback(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.entityRemovedCallback() && query.matches(entity.comp()))
			query.entityRemovedCallback()(entity);
}

void EntityQuerier::entityChangedRemovedCallback(const Entity& oldEntity, ComponentComposition newComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.entityRemovedCallback() && query.matches(oldEntity.comp()) && !query.matches(newComp))
			query.entityRemovedCallback()(oldEntity);
}

void EntityQuerier::entityChangedAddedCallback(const Entity& newEntity, ComponentComposition oldComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.entityAddedCallback() && !query.matches(oldComp) && query.matches(newEntity.comp()))
			query.entityAddedCallback()(newEntity);
}
