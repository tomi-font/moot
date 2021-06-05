#include <Component/Group.hh>
#include <Component/Types.hh>

static_assert(sizeof(ComponentComposition::Bits) * 8 >= std::tuple_size_v<Components>);

void	ComponentGroup::match(Archetype* arch)
{
	if (matches(arch->comp()))
		archs.push_back(arch);
}
