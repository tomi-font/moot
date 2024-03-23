#pragma once

#include <World.hh>
#include <SFML/Graphics/RenderWindow.hpp>

class Game
{
public:

	Game();

	auto* world() { return &m_world; }
	auto* window() { return &m_window; }

private:

	World m_world;
	sf::RenderWindow m_window;
};
