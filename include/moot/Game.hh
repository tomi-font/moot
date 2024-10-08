#pragma once

#include <moot/Window.hh>
#include <moot/World.hh>

class Game
{
public:

	Game();

	World* world() { return &m_world; }

	void play();

private:

	Window m_window;

	World m_world;
};
