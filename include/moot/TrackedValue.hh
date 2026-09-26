#pragma once

#include <moot/Global/ChangeTick.hh>
#include <utility>

template<typename T> class TrackedValue  
{
public:

	explicit TrackedValue(T value = {}) : m_value(std::move(value)) {}

	const T& val() const { return m_value; }
	operator const T&() const { return m_value; }

	T& mut()
	{
		m_lastChangeTick = GlobalChangeTick::current();
		return m_value;
	}
	auto& operator=(T value)
	{
		mut() = std::move(value);
		return *this;
	}

	bool hasChangedSince(GlobalChangeTick::Tick tick) const
	{
		return m_lastChangeTick > tick;
	}

private:

	T m_value;
	GlobalChangeTick::Tick m_lastChangeTick = {};
};
