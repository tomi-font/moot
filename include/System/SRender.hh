#ifndef SRENDER_HH
#define SRENDER_HH

#include "System.hh"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class	SRender : public System
{
public:

	SRender();

	void	render(sf::RenderWindow&);

private:

	sf::Texture	m_texture;
};

#endif
