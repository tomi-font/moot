#pragma once

#include <Entity/Factory.hh>
#include <Window.hh>
#include <World.hh>

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
