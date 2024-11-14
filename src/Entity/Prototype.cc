#include <moot/Entity/Prototype.hh>
#include <moot/util/variant/indexToCompileTime.hh>

void Prototype::add(ComponentVariant&& component)
{
	add(CVId(component), std::move(component));
}

void Prototype::remove(ComponentId cid)
{
	const bool removed = m_components.erase(cid);
	assert(removed);
	m_comp -= cid;
}
