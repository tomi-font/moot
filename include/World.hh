#pragma once

#include <Entity/Archetype.hh>
#include <Entity/Template.hh>
#include <Event/Manager.hh>
#include <System/System.hh>
#include <deque>
#include <memory>
#include <SFML/System/Clock.hpp>

class World
{
public:

	World(sf::RenderWindow*);

	bool isRunning() const { return m_running; }
	void stopRunning() { m_running = false; }

	// Updates all the systems.
	void update();

	void instantiate(const Template&);

private:

	// Existing archetypes, where all the entities' components are.
	// Pointers to Archetypes are stored, so they shall not be invalidated.
	std::deque<Archetype> m_archs;

	// Measures the time elapsed between updates.
	sf::Clock m_clock;

	// All systems, indexed by their IDs.
	std::vector<std::unique_ptr<System>> m_systems;

	EventManager m_eventManager;

	bool m_running;

	// Returns the matching archetype, creating it if it didn't exist.
	Archetype* getArchetype(ComponentComposition);
};
