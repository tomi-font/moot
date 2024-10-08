#pragma once

#include <sol/sol.hpp>

class TemplateStore;
class World;

class GlobalFunctions
{
public:

	static void registerAll(sol::state*, World*, TemplateStore*);
};
