#ifndef WORLD_HH
#define WORLD_HH

#include "Archetype.hh"
#include "System/SRender.hh"
#include "System/SPhysics.hh"
#include "System/SInput.hh"
#include <array>

class	World
{
public:

	World(sf::RenderWindow&);

// let the world turn
	bool	turn();

// returns the archetype matching the components composition
// passed in argument, creating it if it didn't exist
	Archetype*	getArchetype(CsComp);

private:

	std::vector<Archetype>	m_archs;

// reference to the window this world corresponds to
	sf::RenderWindow&	m_window;
// used to measure elapsed time between frames
	sf::Clock			m_clock;

	SInput		m_sinput;
	SPhysics	m_sphysics;
	SRender		m_srender;

// used to perform system-generic stuff in loops
	std::array<System*, System::COUNT>	m_systems;
};

#endif
