#pragma once

#include <moot/Entity/Prototype.hh>
#include <unordered_map>
#include <sol/sol.hpp>

class ComponentAttributes
{
public:

	static void registerAll(sol::state*);

	using Parser = ComponentVariant(*)(const sol::object&);

	template<typename C> static void registerParser(Parser parser) { registerParser(CId<C>, parser); }

	static Parser findParser(const std::string& name) { return s_m_parsers.contains(name) ? s_m_parsers.at(name) : nullptr; }

private:

	static void registerParser(ComponentId, Parser);

	static std::unordered_map<std::string, Parser> s_m_parsers;
};
