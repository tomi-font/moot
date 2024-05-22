#pragma once

#include <Property/Property.hh>
#include <unordered_map>

class PropertyManager
{
public:

	void registerGetter(Property::Identifier, Property::Getter&&);

	void set(Property::Identifier, const Property::Value&);

	Property::Value get(Property::Identifier) const;

private:

	std::unordered_map<Property::Identifier, Property::Value> m_values;
	std::unordered_map<Property::Identifier, Property::Getter> m_getters;
};
