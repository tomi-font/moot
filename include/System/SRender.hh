#ifndef SRENDER_HH
#define SRENDER_HH

#include "System.hh"
#include <SFML/Graphics/Texture.hpp>

class	SRender : public System
{
public:

	SRender();

	bool	update(sf::RenderWindow& window, float elapsedTime) override;

private:

	sf::Texture	m_texture;
};

#endif
