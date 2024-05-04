#pragma once

template<typename T> static constexpr bool isDefaultInitialized(const T& value)
{
	return value == T();
}
