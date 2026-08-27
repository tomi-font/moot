#pragma once

#include <moot/Entity/Prototype.hh>
#include <moot/parsing/sol.hh>

class PrototypeAttributes
{
public:

	static void parse(const std::pair<sol::object, sol::object>&, Prototype*);
};
