#include <moot/Entity/Querier.hh>
#include <moot/Entity/Entity.hh>

void EntityQuerier::match(Archetype* arch)
{
	for (EntityQuery& query : m_queries)
		query.match(arch);
}

void EntityQuerier::onEntityAdded(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityAdded() && query.matches(entity.comp()))
			query.onEntityAdded()(entity);
}

void EntityQuerier::onEntityRemoved(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityRemoved() && query.matches(entity.comp()))
			query.onEntityRemoved()(entity);
}

void EntityQuerier::onChangedEntityRemoved(const Entity& oldEntity, ComponentComposition newComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityRemoved() && query.matches(oldEntity.comp()) && !query.matches(newComp))
			query.onEntityRemoved()(oldEntity);
}

void EntityQuerier::onChangedEntityAdded(const Entity& newEntity, ComponentComposition oldComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityAdded() && !query.matches(oldComp) && query.matches(newEntity.comp()))
			query.onEntityAdded()(newEntity);
}
