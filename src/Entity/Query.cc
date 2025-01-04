#include <moot/Entity/Query.hh>

EntityQuery::EntityQuery(Parameters&& params) :
	m_params(std::move(params))
{
	assert(!m_params.required.empty());
	for (const ComponentComposition& requiredComp : m_params.required)
	{
		assert(!requiredComp.empty());
		assert(requiredComp.hasNoneOf(m_params.forbidden));
	}
}

bool EntityQuery::matches(ComponentComposition comp) const
{
	if (comp.hasNoneOf(m_params.forbidden))
	{
		for (const ComponentComposition& requiredComp : m_params.required)
			if (comp.hasAllOf(requiredComp))
				return true;
	}
	return false;
}

void EntityQuery::match(ComponentCollection* collection)
{
	if (matches(collection->comp()))
		m_matchingCollections.push_back(collection);
}

unsigned EntityQuery::getEntityCount() const
{
	unsigned count = 0;

	for (const ComponentCollection* collection : m_matchingCollections)
		count += collection->size();

	return count;
}
