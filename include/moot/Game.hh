#pragma once

#include <moot/Entity/Factory.hh>
#include <moot/Window.hh>
#include <moot/World.hh>

class Game
{
public:

	Game();

	void create();
	void play();

private:

	Window m_window;

	// Factory must be destroyed after World so that its context is valid when components containing references get destroyed.
	EntityFactory m_factory;
	World m_world;
};
