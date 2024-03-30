#pragma once

#include <World.hh>
#include <sol/forward.hpp>

class Factory
{
public:

	Factory();
	~Factory();

	void populateWorld(World*);

private:

	void spawn(sol::table);

	std::unique_ptr<sol::state> m_lua;
	
	World* m_world;

	std::vector<Template> m_templates;
};
