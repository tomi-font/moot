#pragma once

#include <cassert>

// Returns the number of bits set (to one).
constexpr inline unsigned setBitCount(unsigned n)
{
	return static_cast<unsigned>(__builtin_popcount(n));
}

// Returns the position of the first bit set starting from the LSB.
constexpr inline unsigned firstBitSetPos(unsigned n)
{
	assert(n);
	return static_cast<unsigned>(__builtin_ctz(n));
}
