#pragma once

#include <cstdint>

class GlobalChangeTick
{
public:

	using Tick = std::uint64_t;

	static Tick current() { return s_m_tick; }
	static void advance() { ++s_m_tick; }

private:

	static Tick s_m_tick;
};
