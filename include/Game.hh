#pragma once

#include <Factory.hh>
#include <World.hh>
#include <SFML/Graphics/RenderWindow.hpp>

class Game
{
public:

	Game();

	void create();
	void play();

	auto* world() { return &m_world; }
	auto* window() { return &m_window; }

private:

	World m_world;
	Factory m_factory;

	sf::RenderWindow m_window;
};
