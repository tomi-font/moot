#ifndef SRENDER_HH
#define SRENDER_HH

#include "System.hh"
#include <SFML/Graphics/RenderWindow.hpp>

class	SRender : public System
{
public:

	SRender();

	void	render(sf::RenderWindow& window);
};

#endif
