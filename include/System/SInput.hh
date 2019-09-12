#ifndef SINPUT_HH
#define SINPUT_HH

#include "System.hh"
#include <SFML/Graphics/RenderWindow.hpp>

class	SInput : public System
{
public:

	SInput();

// returns false if quitting was requested by the player
	bool	readInput(sf::RenderWindow&);
};

#endif
