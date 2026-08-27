#pragma once

#include <moot/Entity/Handle.hh>
#include <moot/parsing/sol.hh>

class EntityFunctions
{
public:

	static void registerAll(sol::state*);

	using ComponentGetter = sol::object (*)(const EntityHandle&, lua_State*);

	template<typename C> static void registerComponentGetter(ComponentGetter getter)
	{
		auto& getters = s_m_componentGetters;
		getters.resize(std::max(getters.size(), std::size_t(CId<C>) + 1));
		getters[CId<C>] = getter;
	}

	static ComponentGetter getComponentGetter(ComponentId cId) { return s_m_componentGetters[cId]; }

private:

	static std::vector<ComponentGetter> s_m_componentGetters;
};
