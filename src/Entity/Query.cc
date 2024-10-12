#include <moot/Entity/Query.hh>
#include <moot/Entity/Entity.hh>

static_assert(sizeof(ComponentId) * 8 >= ComponentCount);
static_assert(sizeof(ComponentComposition::Bits) * 8 >= ComponentCount);

EntityQuery::EntityQuery(std::initializer_list<ComponentComposition> required, ComponentComposition forbidden, EntityInitFunction entityInitFunc) :
	m_required(required),
	m_forbidden(forbidden),
	m_entityInitFunc(entityInitFunc)
{
	assert(!m_required.empty());
	for (const ComponentComposition& requiredComp : m_required)
	{
		assert(!requiredComp.empty());
		assert(requiredComp.hasNoneOf(forbidden));
	}
}

bool EntityQuery::matches(ComponentComposition comp) const
{
	if (comp.hasNoneOf(m_forbidden))
	{
		for (const ComponentComposition& requiredComp : m_required)
			if (comp.hasAllOf(requiredComp))
				return true;
	}
	return false;
}

void EntityQuery::match(Archetype* arch)
{
	if (matches(arch->comp()))
		m_matchedArchs.push_back(arch);
}

void EntityQuery::initializeEntity(const Entity& entity) const
{
	if (m_entityInitFunc && matches(entity.comp()))
		m_entityInitFunc(entity);
}
