#include <moot/Entity/Manager.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CEntity.hh>
#include <moot/Component/CLocalPosition.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Entity/util.hh>

EntityManager::EntityManager() :
	m_nextEId(1), // 0 (default-constructed value) is the invalid entity ID.
	m_isPreUpdate(false)
{
}

static void checkComponentComposition(ComponentComposable entity)
{
	if (!entity.has<CPosition>())
		assert((entity.hasNoneOf<CCollisionBox, CConvexPolygon, CCamera, CMove, CRigidbody, CPointable, CLight, CLocalPosition>()));

	if (entity.has<CPointable>())
		assert(entity.has<CConvexPolygon>());

	if (entity.has<CLocalPosition>())
		assert(!entity.has<CCollisionBox>());
}

EntityHandle EntityManager::processEntityToSpawn(ComponentCollection* entity, std::optional<sf::Vector2f> pos)
{
	entity->add<CEntity>(m_nextEId++);

	if (pos)
		entity->add<CPosition>(*pos);

	return {{entity, 0}, entity->comp(), this};
}

EntityHandle EntityManager::spawn(const Prototype& proto, std::optional<sf::Vector2f> pos)
{
	assert(proto.size() == 1);
	assert((proto.hasNoneOf<CChildren, CParent>()));

	const EntityHandle entity = processEntityToSpawn(&m_entitiesToSpawn.emplace_back(proto), pos);
	checkComponentComposition(entity);
	return entity;
}

EntityHandle EntityManager::spawnEmpty(std::optional<sf::Vector2f> pos)
{
	return processEntityToSpawn(&m_entitiesToSpawn.emplace_back(), pos);
}

void EntityManager::remove(const EntityHandle& entity)
{
	assert(!isEntityToSpawn(entity));
	assert(!m_entitiesToChange.contains(entity));
	assert(!m_isPreUpdate);

	m_entitiesToRemove.emplace(entity);
	m_entitiesToExtract[entity.collection].emplace(entity.index);

	if (const CChildren* cChildren = entity.find<CChildren*>())
		for (EntityId childEId : cChildren->eIds())
			remove(makeHandle(m_entityIdMap.at(childEId)));
}

EntityPointer EntityManager::getEntityPointer(EntityId eId)
{
	try
	{
		return m_entityIdMap.at(eId);
	}
	catch (const std::out_of_range&)
	{
		for (ComponentCollection& collection : m_entitiesToSpawn)
		{
			const EntityPointer entity = {&collection, 0};
			if (Entity::getId(entity) == eId)
				return entity;
		}
		assert(false);
	}
}

EntityHandle EntityManager::getEntity(EntityId eId)
{
	return makeHandle(getEntityPointer(eId));
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

bool EntityManager::isEntityToSpawn(const EntityPointer& entity) const
{
	if (entity.index == 0)
	{
		for (const ComponentCollection& collection : m_entitiesToSpawn)
			if (&collection == entity.collection)
				return true;
	}
	return false;
}

EntityManager::EntityToChange* EntityManager::registerEntityToChange(const EntityPointer& entity)
{
	assert(!m_isPreUpdate);
	assert(!m_entitiesToRemove.contains(entity));

	m_entitiesToExtract[entity.collection].emplace(entity.index);
	return &m_entitiesToChange[entity];
}

ComponentCollection* EntityManager::addComponentTo(const EntityPointer& entity, ComponentId cId)
{
	if (isEntityToSpawn(entity))
	{
		checkComponentComposition(entity.comp() += cId);
		return entity.collection;
	}

	EntityToChange* entityToChange = registerEntityToChange(entity);
	assert(!entityToChange->toRemove.has(cId));
	checkComponentComposition(entity.comp() + ComponentComposition(cId) + entityToChange->toAdd.comp() - entityToChange->toRemove);

	return &entityToChange->toAdd;
}

void EntityManager::removeComponentFrom(const EntityPointer& entity, ComponentId cId)
{
	assert(cId != CId<CEntity>);
	assert(cId != CId<CChildren>);
	assert(cId != CId<CParent>);

	if (isEntityToSpawn(entity))
	{
		entity.collection->remove(cId);
		checkComponentComposition(*entity.collection);
		return;
	}

	EntityToChange* entityToChange = registerEntityToChange(entity);
	entityToChange->toRemove += cId;
	assert(!entityToChange->toAdd.has(cId));
	checkComponentComposition(entity.comp() + entityToChange->toAdd.comp() - entityToChange->toRemove);
}

EntityManager::PreUpdateInfo EntityManager::preUpdateEntities()
{
	std::unordered_map<EntityPointer, ComponentComposition> entitiesToChange;
	for (const auto& [entity, entityToChange] : m_entitiesToChange)
	{
		entitiesToChange.emplace(entity, entity.comp() + entityToChange.toAdd.comp() - entityToChange.toRemove);
	}

	m_isPreUpdate = true;
	return { m_entitiesToRemove, entitiesToChange };
}

EntityManager::UpdateInfo EntityManager::updateEntities()
{
	m_isPreUpdate = false;

	UpdateInfo updateInfo;

	std::unordered_map<ComponentCollection*, ComponentComposition> changedEntities;

	for (EntityPointer ePtr : m_entitiesToRemove)
	{
		const EntityHandle entity = makeHandle(ePtr);
		const EntityId eId = Entity::getId(entity);

		// Remove the link to the child being removed from the topmost parent not being removed.
		if (CParent* cParent = entity.find<CParent*>())
		{
			if (const auto parentIt = m_entityIdMap.find(cParent->eId()); parentIt != m_entityIdMap.end())
			{
				const EntityPointer& parent = parentIt->second;
				if (!m_entitiesToRemove.contains(parent))
				{
					parent.get<CChildren*>()->remove(eId);
				}
			}
		}

		const bool erased = m_entityIdMap.erase(eId);
		assert(erased);
	}
	m_entitiesToRemove.clear();

	for (auto& [entity, entityToChange] : m_entitiesToChange)
	{
		ComponentCollection& entityToSpawn = m_entitiesToSpawn.emplace_back(std::move(entityToChange.toAdd));
		entityToSpawn.add(entity.comp() -= entityToChange.toRemove, entity.collection, entity.index);
		checkComponentComposition(entityToSpawn);
		changedEntities.emplace(&entityToSpawn, entity.comp());
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

	for (ComponentCollection& entityToSpawn : m_entitiesToSpawn)
	{
		const auto& [collectionIt, inserted] = m_collections.emplace(entityToSpawn.comp().bits(), std::move(entityToSpawn));
		ComponentCollection& collection = collectionIt->second;
		if (inserted)
			updateInfo.newCollections.emplace(&collection);
		else
			collection.append(std::move(entityToSpawn));

		const EntityPointer addedEntity = {&collection, collection.size() - 1};
		const EntityId eId = Entity::getId(addedEntity);

		m_entityIdMap[eId] = addedEntity;
		
		if (const auto changedEntityIt = changedEntities.find(&entityToSpawn); changedEntityIt != changedEntities.end())
			updateInfo.changedEntities.emplace(addedEntity, changedEntityIt->second);
		else
			updateInfo.addedEntities.insert(addedEntity);
	}
	m_entitiesToSpawn.clear();

	assert(updateInfo.changedEntities.size() == changedEntities.size());
	return updateInfo;
}
