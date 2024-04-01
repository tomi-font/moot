#include <Entity/Template.hh>
#include <utility/variant/indexToCompileTime.hh>

void Template::add(ComponentVariant&& component)
{
	add(CVId(component), std::move(component));
}

void Template::remove(ComponentId cid)
{
	const bool removed = m_components.erase(cid);
	assert(removed);
	m_comp -= cid;
}
