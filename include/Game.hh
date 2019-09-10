#ifndef GAME_HH
#define GAME_HH

#include "World.hh"

class	Game
{
public:

	Game();

	bool	play() { return m_world.turn(); }

private:

	World	m_world;

	sf::RenderWindow	m_window;
};

#endif
