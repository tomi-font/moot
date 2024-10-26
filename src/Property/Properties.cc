#include <moot/Property/Properties.hh>
#include <cassert>

void Properties::registerGetter(std::string&& name, Property::Getter&& getter)
{
	assert(!m_values.contains(name));
	assert(!m_getters.contains(name));
	m_getters.emplace(std::move(name), std::move(getter));
}

void Properties::set(const std::string& name, Property::Value&& value)
{
	assert(!m_getters.contains(name));
	m_values[name] = std::move(value);
}

Property::Value Properties::get(const std::string& name) const
{
	if (auto getterIt = m_getters.find(name); getterIt != m_getters.end())
		return getterIt->second();
	else
		return m_values.at(name);
}
