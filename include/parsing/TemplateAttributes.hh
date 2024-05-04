#pragma once

#include <Entity/Template.hh>
#include <sol/sol.hpp>

class TemplateAttributes
{
public:

	static void parse(const std::pair<sol::object, sol::object>&, Template*);
};
