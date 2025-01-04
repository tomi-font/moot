#pragma once

#include <moot/Entity/Handle.hh>
#include <sol/sol.hpp>

class EntityFunctions
{
public:

	static void registerAll(sol::state*);

	using ComponentGetter = sol::object (*)(const EntityHandle&, lua_State*);

	template<typename C> static void registerComponentGetter(ComponentGetter getter)
	{
		s_m_componentGetters[CId<C>] = getter;
	}

	static ComponentGetter getComponentGetter(ComponentId cId) { return s_m_componentGetters[cId]; }

private:

	static std::vector<ComponentGetter> s_m_componentGetters;
};
