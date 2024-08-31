#pragma once

#include <sol/sol.hpp>

class TemplateStore;
class World;

class GlobalFunctions
{
public:

	static void registerPrePopulating(sol::state*, World*, TemplateStore*);
	static void registerPostPopulating(sol::state*, World*);
};
