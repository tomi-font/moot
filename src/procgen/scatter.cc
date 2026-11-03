#include <moot/procgen/scatter.hh>
#include <moot/procgen/hash.hh>
#include <cassert>
#include <climits>
#include <cmath>

std::vector<Vector2f> jitteredGrid(unsigned size, unsigned seed)
{
	assert(size > 0 && size < std::sqrt(UINT_MAX));

	std::vector<Vector2f> points;
	points.reserve(size * size);

	for (unsigned row = 0; row != size; ++row)
		for (unsigned column = 0; column != size; ++column)
			points.emplace_back((column + hashToUnit(column, row, seed)) / size,
			                    (row + hashToUnit(column, row, seed + 1)) / size);

	return points;
}
