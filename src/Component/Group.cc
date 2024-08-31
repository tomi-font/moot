#include <moot/Component/Group.hh>

static_assert(sizeof(ComponentId) * 8 >= ComponentCount);
static_assert(sizeof(ComponentComposition::Bits) * 8 >= ComponentCount);

ComponentGroup::ComponentGroup(std::initializer_list<ComponentComposition> required, ComponentComposition forbidden, bool initializesEntities) :
	m_required(required),
	m_forbidden(forbidden),
	m_initializesEntities(initializesEntities)
{
	assert(!m_required.empty());
	for (const ComponentComposition& requiredComp : m_required)
	{
		assert(!requiredComp.empty());
		assert(requiredComp.hasNoneOf(forbidden));
	}
}

void ComponentGroup::match(Archetype* arch)
{
	if (matches(arch->comp()))
		m_archs.push_back(arch);
}

bool ComponentGroup::matches(ComponentComposition comp) const
{
	if (comp.hasNoneOf(m_forbidden))
	{
		for (const ComponentComposition& requiredComp : m_required)
			if (comp.hasAllOf(requiredComp))
				return true;
	}
	return false;
}
