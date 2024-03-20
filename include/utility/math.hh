#pragma once

#include <cmath>
#include <limits>

// Returns the absolute difference of two numbers.
template<typename T> T absDiff(T left, T right)
{
	return std::abs(left - right);
}

// Returns the maximum of the absolute values of A and B.
template<typename T> T maxAbs(T a, T b)
{
	return std::max(std::abs(a), std::abs(b));
}

// Returns whether two floating-point numbers are equal.
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool equal(T left, T right)
{
	return absDiff(left, right) < std::numeric_limits<T>::epsilon() * maxAbs(left, right);
}

// Returns 1, -1 or 0 depending on whether num is (strictly) positive, negative, or zero.
template<typename T> int normalize(T num)
{
	if (num > 0)
		return 1;
	else if (num < 0)
		return -1;
	else
		return 0;
}
