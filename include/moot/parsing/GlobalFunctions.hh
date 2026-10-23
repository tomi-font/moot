#pragma once

#include <sol/sol.hpp>

class Game;

class GlobalFunctions
{
public:

	static void registerAll(sol::state*, Game*);
};
