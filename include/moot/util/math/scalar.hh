#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

// Returns the absolute difference of two numbers.
template<typename T> T absDiff(T left, T right)
{
	return std::abs(left - right);
}

// Returns the maximum of the absolute values of the given numbers.
template<typename T> T maxAbs(T first, std::same_as<T> auto... rest)
{
	return std::max({std::abs(first), std::abs(rest)...});
}

// Returns the epsilon of the given numbers.
template<typename T> T epsilon(T first, std::same_as<T> auto... rest)
{
	return maxAbs(first, rest...) * std::numeric_limits<T>::epsilon();
}

// Returns whether two floating-point numbers are within the given tolerance.
template<std::floating_point T> bool equal(T left, T right, T tolerance)
{
	return absDiff(left, right) <= tolerance;
}

// Returns whether two floating-point numbers are considered equal.
template<std::floating_point T> bool equal(T left, T right)
{
	return equal(left, right, epsilon(left, right));
}

// Returns 1, -1 or 0 depending on whether num is (strictly) positive, negative, or zero.
template<typename T> constexpr int sign(T num)
{
	if (num > 0)
		return 1;
	else if (num < 0)
		return -1;
	else
		return 0;
}
