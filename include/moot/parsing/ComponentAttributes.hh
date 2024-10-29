#pragma once

#include <moot/Entity/Prototype.hh>
#include <unordered_map>
#include <sol/sol.hpp>

class ComponentAttributes
{
public:

	static void registerAll(sol::state*);

	using Parser = ComponentVariant(*)(const sol::object&);

	static Parser findParser(const std::string_view& name) { return s_m_parsers.contains(name) ? s_m_parsers.at(name) : nullptr; }

private:

	static const std::unordered_map<std::string_view, Parser> s_m_parsers;
};
