#include <Component/Group.hh>

static_assert(sizeof(ComponentId) * 8 >= std::tuple_size_v<Components>);
static_assert(sizeof(ComponentComposition::Bits) * 8 >= std::tuple_size_v<Components>);

ComponentGroup::ComponentGroup(ComponentComposition required, ComponentComposition forbidden, bool initializesEntities) :
	m_required(required),
	m_forbidden(forbidden),
	m_initializesEntities(initializesEntities)
{
	assert(required.hasNoneOf(forbidden));
}

void ComponentGroup::match(Archetype* arch)
{
	assert(m_required.count());

	if (matches(arch->comp()))
		m_archs.push_back(arch);
}

bool ComponentGroup::matches(ComponentComposition comp) const
{
	return comp.hasAllOf(m_required) && comp.hasNoneOf(m_forbidden);
}
