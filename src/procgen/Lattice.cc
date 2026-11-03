#include <moot/procgen/Lattice.hh>
#include <moot/procgen/hash.hh>
#include <cassert>
#include <climits>
#include <cmath>

Lattice::Lattice(unsigned size, unsigned seed) :
	m_size(size),
	m_values(size * size)
{
	assert(size > 0 && size < std::sqrt(UINT_MAX));

	for (unsigned y = 0; y != size; ++y)
		for (unsigned x = 0; x != size; ++x)
			m_values[y * size + x] = hashToUnit(x, y, seed);
}

float Lattice::at(float x, float y) const
{
	x = (x - std::floor(x)) * m_size;
	y = (y - std::floor(y)) * m_size;
	const unsigned x0 = static_cast<unsigned>(x) % m_size;
	const unsigned y0 = static_cast<unsigned>(y) % m_size;
	const unsigned x1 = (x0 + 1) % m_size;
	const unsigned y1 = (y0 + 1) % m_size;

	// Quintic smoothing keeps the blend's slope and curvature continuous across the lattice points.
	const auto smooth = [](float t) { return t * t * t * (t * (t * 6 - 15) + 10); };
	const float fx = smooth(x - std::floor(x));
	const float fy = smooth(y - std::floor(y));

	const float bottom = std::lerp(m_values[y0 * m_size + x0], m_values[y0 * m_size + x1], fx);
	const float top = std::lerp(m_values[y1 * m_size + x0], m_values[y1 * m_size + x1], fx);
	return std::lerp(bottom, top, fy);
}
