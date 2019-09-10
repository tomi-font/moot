#ifndef WORLD_HH
#define WORLD_HH

#include "Archetype.hh"
#include "System/SRender.hh"
#include <array>

class	World
{
public:

	World(sf::RenderWindow&);

// let the world turn
	bool	turn();

// returns the archetype matching the components composition
// passed in argument, creating it if it didn't exist
	Archetype&	getArchetype(t_Comp);

private:

	std::vector<Archetype>	m_archs;

// we populate an array of System* to perform system-generic work in loops
	std::array<System*, System::COUNT> m_systems;

// reference to the window this world corresponds to
	sf::RenderWindow&	m_window;

	SRender	m_srender;
};

#endif
