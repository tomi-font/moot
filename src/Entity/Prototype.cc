#include <moot/Entity/Prototype.hh>

void Prototype::add(ComponentVariant&& component)
{
	add(CVId(component), std::move(component));
}

void Prototype::remove(ComponentId cId)
{
	const bool removed = m_components.erase(cId);
	assert(removed);
	m_comp -= cId;
}
