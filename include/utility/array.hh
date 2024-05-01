#pragma once

#include <ranges>

template<typename T, auto N> consteval bool hasDefaultInitializedElement(T (&array)[N])
{
	for (auto i : std::views::iota(0u, N))
	{
		if (array[i] == T())
			return true;
	}
	return false;
}
