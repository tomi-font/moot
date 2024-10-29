#pragma once

#include <moot/Entity/Prototype.hh>
#include <sol/sol.hpp>

class PrototypeAttributes
{
public:

	static void parse(const std::pair<sol::object, sol::object>&, Prototype*);
};
