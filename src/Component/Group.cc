#include <Component/Group.hh>

void	ComponentGroup::match(Archetype* arch)
{
	if (matches(arch->getComp()))
		archs.push_back(arch);
}
