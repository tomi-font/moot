#pragma once

#include <moot/utility/value.hh>
#include <ranges>

template<unsigned N> consteval bool hasDefaultInitializedElement(auto (&array)[N], std::initializer_list<unsigned> exceptions = {})
{
	for (unsigned i : std::views::iota(0u, N))
	{
		if (isDefaultInitialized(array[i]))
		{
			for (auto exceptionIt = exceptions.begin();; ++exceptionIt)
			{
				if (exceptionIt == exceptions.end())
					return true;
				else if (*exceptionIt == i)
					break;
			}
		}
	}
	return false;
}
