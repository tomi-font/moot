#include <Property/Manager.hh>
#include <cassert>

void PropertyManager::registerGetter(Property::Identifier pid, Property::Getter&& getter)
{
	assert(!m_values.contains(pid));
	assert(!m_getters.contains(pid));
	m_getters[pid] = std::move(getter);
}

Property::Value PropertyManager::get(Property::Identifier pid) const
{
	if (m_getters.contains(pid))
	{
		assert(!m_values.contains(pid));
		return m_getters.at(pid)();
	}
	else
		return m_values.at(pid);
}

void PropertyManager::set(Property::Identifier pid, const Property::Value& value)
{
	assert(!m_getters.contains(pid));
	assert(m_values.contains(pid));
	m_values[pid] = value;
}
