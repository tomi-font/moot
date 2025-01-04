#include <moot/Entity/Manager.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CEntity.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CView.hh>
#include <moot/Entity/util.hh>

EntityManager::EntityManager() :
	m_nextEId(1) // 0 (default-constructed value) is the invalid entity ID.
{
}

static void checkEntityToAdd(const ComponentCollection& entity)
{
	if (!entity.has<CPosition>())
		assert((entity.hasNoneOf<CCollisionBox, CConvexPolygon, CView, CMove, CRigidbody, CPointable>()));

	if (entity.has<CPointable>())
		assert(entity.has<CConvexPolygon>());
}

EntityHandle EntityManager::spawn(const Prototype& proto, std::optional<std::reference_wrapper<const sf::Vector2f>> pos)
{
	assert(proto.size() == 1);
	assert((proto.hasNoneOf<CChildren, CParent>()));

	ComponentCollection& entity = m_entitiesToAdd.emplace_back(proto);

	entity.add<CEntity>(m_nextEId++);

	if (pos)
		entity.add<CPosition>(*pos);

	checkEntityToAdd(entity);

	return {{&entity, 0}, entity.comp(), this};
}

void EntityManager::remove(const EntityHandle& entity)
{
	assert(!isEntityToAdd(entity));
	assert(!m_entitiesToChange.contains(entity));

	m_entitiesToRemove.emplace(entity);
	m_entitiesToExtract[entity.collection].emplace(entity.index);
	m_entityInfo.entitiesToRemove.emplace(entity);

	if (const CChildren* cChildren = entity.find<CChildren*>())
		for (EntityId childEId : cChildren->eIds())
			remove(makeHandle(m_entityIdMap.at(childEId)));
}

EntityHandle EntityManager::getEntity(EntityId eId)
{
	try
	{
		return makeHandle(m_entityIdMap.at(eId));
	}
	catch (const std::out_of_range&)
	{
		for (ComponentCollection& collection : m_entitiesToAdd)
		{
			const EntityPointer entity = {&collection, 0};
			if (Entity::getId(entity) == eId)
				return makeHandle(entity);
		}
		assert(false);
	}
}

EntityHandle EntityManager::makeHandle(EntityPointer entity)
{
	ComponentComposition comp = entity.comp();
	if (const auto entityToChangeIt = m_entitiesToChange.find(entity); entityToChangeIt != m_entitiesToChange.end())
	{
		comp += entityToChangeIt->second.toAdd.comp();
		comp -= entityToChangeIt->second.toRemove;
	}
	return {entity, comp, this};
}

bool EntityManager::isEntityToAdd(const EntityPointer& entity) const
{
	if (entity.index == 0)
	{
		for (const ComponentCollection& collection : m_entitiesToAdd)
			if (&collection == entity.collection)
				return true;
	}
	return false;
}

std::pair<EntityManager::EntityToChange*, ComponentComposition*> EntityManager::registerEntityToChange(const EntityPointer& entity)
{
	assert(!m_entitiesToRemove.contains(entity));
	m_entitiesToExtract[entity.collection].emplace(entity.index);
	return {&m_entitiesToChange[entity], &m_entityInfo.entitiesToChange.emplace(entity, entity.collection->comp()).first->second};
}

ComponentCollection* EntityManager::addComponentTo(const EntityPointer& entity, ComponentId cId)
{
	if (isEntityToAdd(entity))
		return entity.collection;

	auto [entityToChange, newComp] = registerEntityToChange(entity);
	assert(!entityToChange->toRemove.has(cId));
	*newComp += cId;

	return &entityToChange->toAdd;
}

void EntityManager::removeComponentFrom(const EntityPointer& entity, ComponentId cId)
{
	if (isEntityToAdd(entity))
	{
		entity.collection->remove(cId);
		return;
	}

	auto [entityToChange, newComp] = registerEntityToChange(entity);
	assert(!entityToChange->toAdd.has(cId));
	*newComp -= cId;

	assert(cId != CId<CEntity>);
	assert(cId != CId<CChildren>);
	assert(cId != CId<CParent>);

	entityToChange->toRemove += cId;
}

void EntityManager::updateEntities()
{
	m_entityInfo.entitiesToRemove.clear();
	m_entityInfo.entitiesToChange.clear();
	m_entityInfo.newCollections.clear();
	m_entityInfo.addedEntities.clear();
	m_entityInfo.changedEntities.clear();

	std::unordered_map<ComponentCollection*, ComponentComposition> changedEntities;

	for (EntityPointer ePtr : m_entitiesToRemove)
	{
		const EntityHandle entity = makeHandle(ePtr);
		const EntityId eId = Entity::getId(entity);

		if (CParent* cParent = entity.find<CParent*>())
		{
			const EntityPointer parent = m_entityIdMap.at(*cParent);
			parent.get<CChildren*>()->remove(eId);
		}

		const bool erased = m_entityIdMap.erase(eId);
		assert(erased);
	}
	m_entitiesToRemove.clear();

	for (const auto& [entity, entityToChange] : m_entitiesToChange)
	{
		ComponentCollection& entityToAdd = m_entitiesToAdd.emplace_back(std::move(entityToChange.toAdd));
		entityToAdd.add(entity.comp() -= entityToChange.toRemove, std::move(*entity.collection), entity.index);
		checkEntityToAdd(entityToAdd);
		changedEntities.emplace(&entityToAdd, entity.comp());
	}
	m_entitiesToChange.clear();

	for (const auto& [collection, removedEntitiesIndices] : m_entitiesToExtract)
	{
		assert(*removedEntitiesIndices.rbegin() <= *removedEntitiesIndices.begin());

		for (unsigned index : removedEntitiesIndices)
			collection->erase(index);

		const auto& entityComponents = collection->getAll<CEntity>();
		for (unsigned i = *removedEntitiesIndices.rbegin(); i != collection->size(); ++i)
		{
			m_entityIdMap.at(entityComponents[i].eId()).index = i;
		}
	}
	m_entitiesToExtract.clear();

	assert(EntityPointer::instanceCount() == m_entityIdMap.size());

	for (ComponentCollection& entityToAdd : m_entitiesToAdd)
	{
		const auto& [collectionIt, inserted] = m_collections.emplace(entityToAdd.comp().bits(), std::move(entityToAdd));
		ComponentCollection& collection = collectionIt->second;
		if (inserted)
			m_entityInfo.newCollections.emplace(&collection);
		else
			collection.append(std::move(entityToAdd));

		const EntityPointer addedEntity = {&collection, collection.size() - 1};
		const EntityId eId = Entity::getId(addedEntity);

		m_entityIdMap[eId] = addedEntity;
		
		if (const auto changedEntityIt = changedEntities.find(&entityToAdd); changedEntityIt != changedEntities.end())
			m_entityInfo.changedEntities.emplace(addedEntity, changedEntityIt->second);
		else
			m_entityInfo.addedEntities.insert(addedEntity);
	}
	m_entitiesToAdd.clear();
	assert(m_entityInfo.changedEntities.size() == changedEntities.size());
}
