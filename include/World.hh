#pragma once

#include <Entity/Archetype.hh>
#include <Entity/Template.hh>
#include <Event/Manager.hh>
#include <System/System.hh>
#include <deque>
#include <memory>
#include <unordered_set>
#include <SFML/System/Clock.hpp>

template<> struct std::hash<EntityContext>
{
	inline std::size_t operator()(const EntityContext&) const;
};

class World
{
public:

	World(sf::RenderWindow*);

	bool isRunning() const { return m_running; }
	void stopRunning() { m_running = false; }

	void update();

	void instantiate(Template&& temp) { m_entitiesToInstantiate.push_back(std::move(temp)); }
	void remove(EntityContext&& entity);

	void addComponentTo(EntityContext*, ComponentVariant&&);
	void removeComponentFrom(EntityContext*, ComponentId);

	// Finds an entity by its name.
	EntityContext findEntity(std::string_view);

	ComponentVariant* getStagedComponentOf(const EntityContext&, ComponentId cid);

private:

	void updateEntitiesComponents();
	void updateEntities();

	Archetype* findArchetype(ComponentComposition);
	Archetype* getArchetype(ComponentComposition);

	// Existing archetypes, where all the entities' components are.
	// Pointers to Archetypes are stored, so they shall not be invalidated.
	std::deque<Archetype> m_archs;

	// Measures the time elapsed between updates.
	sf::Clock m_clock;

	// All systems, indexed by their IDs.
	std::vector<std::unique_ptr<System>> m_systems;

	EventManager m_eventManager;

	// Entities and components are added/removed asynchronously to prevent
	// Archetypes getting modified while the Systems are iterating through them.
	std::vector<Template> m_entitiesToInstantiate;
	std::set<EntityContext, std::greater<EntityContext>> m_entitiesToRemove; // Sorted in descending order so that entities that are removed first do not invalidate the indices of the others.
	std::unordered_map<EntityContext, std::unordered_set<ComponentId>> m_componentsToRemove;
	std::unordered_map<EntityContext, std::unordered_map<ComponentId, ComponentVariant>> m_componentsToAdd;

	ComponentGroup m_namedEntities;

	bool m_running;
};
