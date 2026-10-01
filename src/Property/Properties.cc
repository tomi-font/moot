#include <moot/Property/Properties.hh>
#include <cassert>

void Properties::registerGetter(const std::string& name, Property::Getter&& getter)
{
	assert(!m_values.contains(name));
	assert(!m_getters.contains(name));
	m_getters.emplace(name, std::move(getter));
}

void Properties::set(const std::string& name, Property::Value&& value)
{
	assert(!m_getters.contains(name));
	m_values[name] = std::move(value);
}

std::optional<Property::Value> Properties::find(const std::string& name) const
{
	if (auto getterIt = m_getters.find(name); getterIt != m_getters.end())
		return getterIt->second();
	else if (auto valueIt = m_values.find(name); valueIt != m_values.end())
		return valueIt->second;
	else
		return std::nullopt;
}
