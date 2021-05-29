#ifndef WORLD_HH
#define WORLD_HH

#include "Archetype.hh"
#include "System/System.hh"
#include "Event/Listener.hh"
#include <deque>

class	World : EventListener
{
public:

	World();

	bool	isRunning() const { return m_running; }

// Updates all the systems, using the passed window.
	void	update(sf::RenderWindow&);

// Returns the matching archetype, creating it if it didn't exist.
	Archetype*	getArchetype(CsComp);

private:

// Existing archetypes, where all the entities' components are.
	std::deque<Archetype>	m_archs;

// Used to measure the time elapsed between updates.
	sf::Clock	m_clock;

// Whether it is running or has stopped.
	bool 		m_running;

// All systems.
	std::vector<std::unique_ptr<System>> m_systems;

	EventManager m_eventManager;

	void triggered(const Event&) override;
};

#endif
