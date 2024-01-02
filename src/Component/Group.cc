#include "Component/Composition.hh"
#include <Component/Group.hh>
#include <Archetype.hh>

static_assert(sizeof(ComponentComposition::Bits) * 8 >= std::tuple_size_v<Components>);

void ComponentGroup::match(Archetype* arch)
{
	if (matches(arch->comp()))
		m_archs.push_back(arch);
}

bool ComponentGroup::matches(ComponentComposition comp) const
{
	// The composition matches if all of the included and none of the excluded components are present.
	return (comp & m_required) == m_required && (comp & m_forbidden) == ComponentComposition();
}
