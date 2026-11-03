#pragma once

#include <cassert>
#include <span>
#include <vector>

// A square grid of values that wraps around at its edges.
class Field
{
public:

	class Resampler;

	Field(unsigned size, float value = 0);

	auto& size() const { return m_size; }
	auto& values() { return m_values; }

	float& at(unsigned x, unsigned y) { return m_values[std::size_t(y) * m_size + x]; }
	float at(unsigned x, unsigned y) const { return m_values[std::size_t(y) * m_size + x]; }

	// The index along a row or column of the value `index` would reach, wrapping around the edges once.
	unsigned wrap(int index) const
	{
		assert(index >= -int(m_size) && index < 2 * int(m_size));
		if (index < 0)
			return index + int(m_size);
		if (unsigned(index) >= m_size)
			return index - m_size;
		return index;
	}

	// Stretches the values over [0, 1].
	void stretchToUnit();

private:

	// Where a coordinate falls between two grid points, wrapping around: the point before, the one after, and the way between.
	struct Between
	{
		unsigned before;
		unsigned after;
		float fraction;
	};

	static Between between(float coordinate, unsigned size);

	unsigned m_size;
	std::vector<float> m_values;
};

// Blends a field to a grid of another size one row at a time, so that nothing the size of that grid is kept:
// the field's rows are widened to the new size once, and a row asked for is blended from the two around it.
class Field::Resampler
{
public:

	Resampler(const Field& field, unsigned size);

	auto& size() const { return m_size; }

	// Writes row `y` of the resampled grid to `row`, `size()` values.
	void writeRow(unsigned y, std::span<float> row) const;

private:

	unsigned m_size;
	std::vector<Between> m_betweens; // Where each row and column of the new grid falls between the field's.
	std::vector<float> m_wideRows;   // The field's rows, each widened to the new size.
};
