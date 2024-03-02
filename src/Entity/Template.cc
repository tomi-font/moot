#include <Entity/Template.hh>
#include <utility/variant/indexToCompileTime.hh>

void Template::add(ComponentVariant&& component)
{
	const ComponentId cid = static_cast<ComponentId>(component.index());
	assert(!has(cid));
	m_comp += cid;

	const bool added = m_components.insert(std::move(component)).second;
	assert(added);
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
