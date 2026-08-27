#pragma once

#include <moot/parsing/sol.hh>

class Game;

class GlobalFunctions
{
public:

	static void registerAll(sol::state*, Game*);
};
