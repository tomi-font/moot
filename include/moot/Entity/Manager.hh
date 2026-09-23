#pragma once

#include <moot/Component/Collection.hh>
#include <moot/Entity/Id.hh>
#include <moot/Entity/Pointer/operators.hh>
#include <moot/Entity/Prototype.hh>
#include <deque>
#include <optional>
#include <set>
#include <unordered_set>
#include <SFML/System/Vector2.hpp>

struct EntityHandle;

class EntityManager
{
public:

	EntityManager();

	EntityHandle spawn(const Prototype&, std::optional<sf::Vector2f> = {});
	EntityHandle spawnEmpty(std::optional<sf::Vector2f> = {});
	void remove(const EntityHandle&);
	bool isEntityToRemove(const EntityPointer& entity) const { return m_entitiesToRemove.contains(entity); }

	EntityPointer getEntityPointer(EntityId);
	EntityHandle getEntity(EntityId);
	EntityHandle makeHandle(EntityPointer);

	template<typename C> C* addComponentTo(const EntityPointer& entity, auto&&... args)
	{
		return addComponentTo(entity, CId<C>)->template add<C>(std::forward<decltype(args)>(args)...);
	}
	ComponentCollection* addComponentTo(const EntityPointer&, ComponentId);
	ComponentCollection* getComponentsToAddOf(const EntityPointer& entity) { return &m_entitiesToChange.at(entity).toAdd; }
	void removeComponentFrom(const EntityPointer&, ComponentId);

protected:

	struct PreUpdateInfo
	{
		std::unordered_set<EntityPointer> entitiesToRemove;
		std::unordered_map<EntityPointer, ComponentComposition> entitiesToChange;
	};
	PreUpdateInfo preUpdateEntities();

	struct UpdateInfo
	{
		std::unordered_set<ComponentCollection*> newCollections;
		std::unordered_set<EntityPointer> addedEntities;
		std::unordered_map<EntityPointer, ComponentComposition> changedEntities;
	};
	UpdateInfo updateEntities();

private:

	struct EntityToChange
	{
		ComponentCollection toAdd;
		ComponentComposition toRemove;
	};

	bool isEntityToSpawn(const EntityPointer&) const;
	EntityHandle processEntityToSpawn(ComponentCollection* entity, std::optional<sf::Vector2f> pos);

	EntityToChange* registerEntityToChange(const EntityPointer&);

	std::unordered_map<ComponentComposition::Bits, ComponentCollection> m_collections;
	std::unordered_map<EntityId, EntityPointer> m_entityIdMap;
	EntityId m_nextEId;

	std::deque<ComponentCollection> m_entitiesToSpawn;
	std::unordered_map<ComponentCollection*, std::set<unsigned, std::greater<>>> m_entitiesToExtract;
	std::unordered_set<EntityPointer> m_entitiesToRemove;
	std::unordered_map<EntityPointer, EntityToChange> m_entitiesToChange;

	bool m_isPreUpdate;
};
