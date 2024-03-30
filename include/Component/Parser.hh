#pragma once

#include <Entity/Template.hh>
#include <unordered_map>
#include <sol/sol.hpp>

class ComponentParser
{
public:

	using Data = sol::basic_object<sol::basic_reference<false>>;
	using Parser = void(*)(Template*, const Data&);

	static Parser get(const std::string_view& name) { return s_m_parsers.at(name); }

private:

	static const std::unordered_map<std::string_view, Parser> s_m_parsers;
};
