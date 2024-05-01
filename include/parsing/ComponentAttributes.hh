#pragma once

#include <Entity/Template.hh>
#include <unordered_map>
#include <sol/sol.hpp>

class ComponentAttributes
{
public:

	static void registerAll(sol::state*);

	using Parser = ComponentVariant(*)(const sol::object&);

	static Parser getParser(const std::string_view& name) { return s_m_parsers.at(name); }

	static void parse(const std::pair<sol::object, sol::object>& component, Template*);

private:

	static const std::unordered_map<std::string_view, Parser> s_m_parsers;
};
