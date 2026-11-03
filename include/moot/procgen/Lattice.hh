#pragma once

#include <vector>

// Random values at the points of a square lattice, blended smoothly in between: value noise.
// The lattice wraps around, so points are in turns of it and (x + 1, y) is the same as (x, y).
class Lattice
{
public:

	Lattice(unsigned size, unsigned seed);

	float at(float x, float y) const;

private:

	unsigned m_size;
	std::vector<float> m_values;
};
