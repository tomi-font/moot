#include <moot/procgen/hash.hh>

// Spreads a change of any input bit over the whole output (lowbias32).
static unsigned mix(unsigned h)
{
	h ^= h >> 16;
	h *= 0x7feb352du;
	h ^= h >> 15;
	h *= 0x846ca68bu;
	h ^= h >> 16;
	return h;
}

float hashToUnit(unsigned x, unsigned y, unsigned seed)
{
	return float(mix(x * 73856093u ^ y * 19349663u ^ seed * 83492791u) >> 8) / float(1u << 24);
}

unsigned deriveSeed(unsigned seed, unsigned n)
{
	return mix(n * 73856093u ^ seed * 83492791u);
}
