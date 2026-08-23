#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

// Returns the absolute difference of two numbers.
template<typename T> T absDiff(T left, T right)
{
	return std::abs(left - right);
}

// Returns the maximum of the absolute values of two numbers.
template<typename T> T maxAbs(T a, T b)
{
	return std::max(std::abs(a), std::abs(b));
}

// Returns the epsilon of two numbers.
template<typename T> T epsilon(T a, T b)
{
	return maxAbs(a, b) * std::numeric_limits<T>::epsilon();
}

// Returns whether two floating-point numbers are within the given tolerance.
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool equal(T left, T right, T tolerance)
{
	return absDiff(left, right) <= tolerance;
}

// Returns whether two floating-point numbers are considered equal.
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool equal(T left, T right)
{
	return equal(left, right, epsilon(left, right));
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
