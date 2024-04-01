#pragma once

#include <Entity/Template.hh>
#include <unordered_map>
#include <sol/sol.hpp>

class ComponentParser
{
public:

	static void registerAll(sol::state*);

	using Parser = void(*)(const sol::object&, Template*);

	static Parser get(const std::string_view& name) { return s_m_parsers.at(name); }

private:

	static const std::unordered_map<std::string_view, Parser> s_m_parsers;
};
