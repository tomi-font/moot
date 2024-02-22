#include <Entity/Template.hh>
#include <utility/variant/indexToCompileTime.hh>

Template::Template(const EntityHandle& entity) : ComponentComposable(entity.comp())
{
	for (ComponentId cid : m_comp)
	{
		variantIndexToCompileTime<ComponentVariant>(cid,
			[&](auto I)
			{
				m_components.insert(entity.get<std::tuple_element_t<I, Components>>());
			});
	}
}

void Template::add(ComponentVariant&& cv)
{
	const bool inserted = m_components.insert(std::move(cv)).second;
	assert(inserted);

	m_comp += static_cast<ComponentId>(cv.index());
}

void Template::remove(ComponentId cid)
{
	assert(has(cid));

	for (auto compIt = m_components.begin(); compIt != m_components.end(); ++compIt)
	{
		if ((*compIt).index() == cid)
		{
			m_components.erase(compIt);
			m_comp -= cid;
			return;
		}
	}
	assert(false);
}
