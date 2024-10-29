#pragma once

#include <sol/sol.hpp>

class World;

class GlobalFunctions
{
public:

	static void registerAll(sol::state*, World*);
};
