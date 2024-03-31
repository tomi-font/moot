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

	void registerUniversal();
	void registerWorldSpecific(World*);

	void spawn(World*, sol::table);

	sol::state& m_lua;

	std::vector<Template> m_templates;
};
