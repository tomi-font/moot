#pragma once

#include <cstdint>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

class GlobalClock
{
	static const sf::Clock s_m_clock;

public:

	using Ticks = std::int64_t;

	static constexpr bool TicksAreMicroseconds = true;

	static Ticks ticksSinceStart()
	{
		return s_m_clock.getElapsedTime().asMicroseconds();
	}
};
