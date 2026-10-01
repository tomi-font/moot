#pragma once

#include <moot/Property/Property.hh>
#include <optional>
#include <unordered_map>

class Properties
{
public:

	void registerGetter(const std::string& name, Property::Getter&&);

	void set(const std::string& name, Property::Value&&);

	Property::Value get(const std::string& name) const { return find(name).value(); }
	template<typename T> T get(const std::string& name) const { return std::get<T>(get(name)); }

	std::optional<Property::Value> find(const std::string& name) const;
	template<typename T> std::optional<T> find(const std::string& name) const
	{
		if (const auto value = find(name))
			return std::get<T>(*value);
		else
			return std::nullopt;
	}

private:

	std::unordered_map<std::string, Property::Value> m_values;
	std::unordered_map<std::string, Property::Getter> m_getters;
};
