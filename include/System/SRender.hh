#pragma once

#include <System/System.hh>
#include <SFML/Graphics/Texture.hpp>

class SRender : public System
{
public:

	SRender();

	void update(sf::RenderWindow& window, float elapsedTime) override;
};
