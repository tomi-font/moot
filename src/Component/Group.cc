#include <Component/Group.hh>

static_assert(sizeof(CsComp::Bits) * 8 >= Component::COUNT);

void	ComponentGroup::match(Archetype* arch)
{
	if (matches(arch->comp()))
		archs.push_back(arch);
}
