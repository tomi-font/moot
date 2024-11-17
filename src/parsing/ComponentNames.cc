#include <moot/parsing/ComponentNames.hh>
#include <moot/util/array.hh>
#include <moot/util/static_warn.hh>

std::vector<std::string> ComponentNames::s_m_names;

void ComponentNames::add(ComponentId cid, std::string&& name)
{
	static const std::unordered_set<ComponentId> s_forbiddenCIds =
	{
		CId<CEntity>,
		CId<CCallback>,
		CId<CChildren>,
		CId<CParent>,
	};
	assert(!s_forbiddenCIds.contains(cid));

	s_m_names.resize(ComponentIdRegistry::idCount());

	assert(s_m_names[cid].empty());
	s_m_names[cid] = std::move(name);
}
