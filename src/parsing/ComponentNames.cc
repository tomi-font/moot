#include <moot/parsing/ComponentNames.hh>
#include <moot/Component/CCallback.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CEntity.hh>
#include <moot/Component/CParent.hh>
#include <unordered_set>

std::vector<std::string> ComponentNames::s_m_names;

void ComponentNames::add(ComponentId cId, std::string&& name)
{
	static const std::unordered_set<ComponentId> s_forbiddenCIds =
	{
		CId<CEntity>,
		CId<CCallback>,
		CId<CChildren>,
		CId<CParent>,
	};
	assert(!s_forbiddenCIds.contains(cId));

	s_m_names.resize(ComponentIdRegistry::idCount());

	assert(s_m_names[cId].empty());
	s_m_names[cId] = std::move(name);
}
