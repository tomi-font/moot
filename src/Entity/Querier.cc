#include <moot/Entity/Querier.hh>

void EntityQuerier::match(ComponentCollection* collection)
{
	for (EntityQuery& query : m_queries)
		query.match(collection);
}

void EntityQuerier::onEntityAdded(const EntityPointer& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityAdded() && query.matches(entity.comp()))
			query.onEntityAdded()(entity);
}

void EntityQuerier::onEntityRemoved(const EntityPointer& entity) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityRemoved() && query.matches(entity.comp()))
			query.onEntityRemoved()(entity);
}

void EntityQuerier::onChangedEntityRemoved(const EntityPointer& oldEntity, ComponentComposition newComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityRemoved() && query.matches(oldEntity.comp()) && !query.matches(newComp))
			query.onEntityRemoved()(oldEntity);
}

void EntityQuerier::onChangedEntityAdded(const EntityPointer& newEntity, ComponentComposition oldComp) const
{
	for (const EntityQuery& query : m_queries)
		if (query.onEntityAdded() && !query.matches(oldComp) && query.matches(newEntity.comp()))
			query.onEntityAdded()(newEntity);
}
