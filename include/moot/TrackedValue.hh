#pragma once

#include <moot/Global/Clock.hh>
#include <utility>

template<typename T> class [[gnu::packed]] TrackedValue  
{
public:

	explicit TrackedValue(T value) : m_lastChangeTicks(), m_value(std::move(value)) {}

	operator const T&() const { return m_value; }

	void operator=(T value)
	{
		m_lastChangeTicks = GlobalClock::ticksSinceStart();
		m_value = std::move(value);
	}

	bool hasChangedSince(GlobalClock::Ticks ticks) const { return m_lastChangeTicks > ticks;}

private:

	GlobalClock::Ticks m_lastChangeTicks;
	T m_value;
};
