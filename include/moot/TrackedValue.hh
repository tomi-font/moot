#pragma once

#include <moot/Global/Clock.hh>
#include <utility>

template<typename T> class TrackedValue  
{
public:

	explicit TrackedValue(T value = {}) : m_value(std::move(value)), m_lastChangeTicks() {}

	const T& val() const { return m_value; }
	operator const T&() const { return m_value; }

	T& mut()
	{
		m_lastChangeTicks = GlobalClock::ticksSinceStart();
		return m_value;
	}
	void operator=(T value) { mut() = std::move(value); }

	bool hasChangedSince(GlobalClock::Ticks ticks) const { return m_lastChangeTicks > ticks; }

private:

	T m_value;
	GlobalClock::Ticks m_lastChangeTicks;
};
