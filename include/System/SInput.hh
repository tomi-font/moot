#ifndef SINPUT_HH
#define SINPUT_HH

#include "System.hh"
#include <SFML/Graphics/RenderWindow.hpp>

class	SInput : public System
{
public:

	SInput();

	void	readInput(sf::RenderWindow&);
};

#endif
