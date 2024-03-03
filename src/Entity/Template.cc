#include <Entity/Template.hh>
#include <utility/variant/indexToCompileTime.hh>

Template& Template::add(ComponentVariant&& component)
{
	m_comp += CVId(component);
	emplace(std::move(component));
	return *this;
}

void Template::remove(ComponentId cid)
{
	assert(has(cid));

	for (auto compIt = m_components.begin(); compIt != m_components.end(); ++compIt)
	{
		if (CVId(*compIt) == cid)
		{
			m_components.erase(compIt);
			m_comp -= cid;
			return;
		}
	}
	assert(false);
}
