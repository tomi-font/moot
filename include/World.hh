#ifndef WORLD_HH
#define WORLD_HH

#include "Archetype.hh"
#include "System/SRender.hh"
#include "System/SPhysics.hh"
#include "System/SInput.hh"
#include <array>
#include <deque>

class	World
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
// Whether it is still running or has stopped.
	bool 		m_running;

	SInput		m_sinput;
	SPhysics	m_sphysics;
	SRender		m_srender;

// used to perform system-generic stuff in loops
	std::array<System*, System::COUNT>	m_systems;
};

#endif
