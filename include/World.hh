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

	// Finds an entity by its name.
	EntityContext findEntity(const std::string&);


	const sf::RenderWindow& window() const { return m_systems[0]->window(); }

private:

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

	bool m_running;

	// Entities are instantiated/removed asynchronously to prevent them
	// getting modified while the Systems are iterating through them.
	std::vector<Template> m_entitiesToInstantiate;
	std::unordered_set<EntityContext> m_entitiesToRemove;
};
