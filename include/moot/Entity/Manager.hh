#pragma once

#include <moot/Component/Collection.hh>
#include <moot/Entity/Id.hh>
#include <moot/Entity/Pointer/operators.hh>
#include <moot/Entity/Prototype.hh>
#include <deque>
#include <set>
#include <unordered_set>
#include <SFML/System/Vector2.hpp>

struct EntityHandle;

class EntityManager
{
public:

	EntityManager();

	EntityHandle spawn(const Prototype&, std::optional<std::reference_wrapper<const sf::Vector2f>> pos = {});
	void remove(const EntityHandle&);

	EntityHandle getEntity(EntityId);

	EntityHandle makeHandle(EntityPointer);
	template<typename C> inline C* addComponentTo(const EntityPointer& entity, auto&&... args)
	{
		return addComponentTo(entity, CId<C>)->template add<C>(std::forward<decltype(args)>(args)...);
	}
	ComponentCollection* getComponentsToAddOf(const EntityPointer& entity) { return &m_entitiesToChange[entity].toAdd; }
	void removeComponentFrom(const EntityPointer&, ComponentId);

protected:

	void updateEntities();

	struct
	{
		std::unordered_set<EntityPointer> entitiesToRemove;
		std::unordered_map<EntityPointer, ComponentComposition> entitiesToChange;

		std::unordered_set<ComponentCollection*> newCollections;
		std::unordered_set<EntityPointer> addedEntities;
		std::unordered_map<EntityPointer, ComponentComposition> changedEntities;
	}
	m_entityInfo;

private:

	struct EntityToChange
	{
		ComponentCollection toAdd;
		ComponentComposition toRemove;
	};

	bool isEntityToAdd(const EntityPointer&) const;
	std::pair<EntityToChange*, ComponentComposition*> registerEntityToChange(const EntityPointer&);
	ComponentCollection* addComponentTo(const EntityPointer&, ComponentId);

	std::unordered_map<ComponentComposition::Bits, ComponentCollection> m_collections;
	std::unordered_map<EntityId, EntityPointer> m_entityIdMap;
	EntityId m_nextEId;

	std::deque<ComponentCollection> m_entitiesToAdd;
	std::unordered_map<ComponentCollection*, std::set<unsigned, std::greater<>>> m_entitiesToExtract;
	std::unordered_set<EntityPointer> m_entitiesToRemove;
	std::unordered_map<EntityPointer, EntityToChange> m_entitiesToChange;
};
