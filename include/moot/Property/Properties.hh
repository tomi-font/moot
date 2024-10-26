#pragma once

#include <moot/Property/Property.hh>
#include <unordered_map>

class Properties
{
public:

	void registerGetter(std::string&& name, Property::Getter&&);

	void set(const std::string& name, Property::Value&&);

	Property::Value get(const std::string& name) const;
	template<typename T> inline T get(const std::string& name) const
	{
		return std::get<T>(get(name));
	}

private:

	std::unordered_map<std::string, Property::Value> m_values;
	std::unordered_map<std::string, Property::Getter> m_getters;
};
