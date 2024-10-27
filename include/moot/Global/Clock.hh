#pragma once

#include <cstdint>
#include <SFML/System/Clock.hpp>

class GlobalClock
{
	static const sf::Clock s_m_clock;

public:

	using Ticks = uint64_t;

	static auto ticksSinceStart()
	{
		return static_cast<Ticks>(s_m_clock.getElapsedTime().asMicroseconds());
	}
};
