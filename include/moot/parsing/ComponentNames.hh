#pragma once

#include <moot/util/value.hh>
#include <moot/Component/Types.hh>

class ComponentNames
{
public:

	template<typename C> static void add(std::string&& name) { add(CId<C>, std::move(name)); }

	template<typename C> static auto& get() { return get(CId<C>); }

	static const std::string& get(ComponentId cid) { return s_m_names[cid]; }

private:

	static void add(ComponentId, std::string&&);

	static std::vector<std::string> s_m_names;
};
