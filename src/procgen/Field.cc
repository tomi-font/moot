#include <moot/procgen/Field.hh>
#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>

Field::Field(unsigned size, float value) :
	m_size(size),
	m_values(size * size, value)
{
	assert(size > 0 && size < std::sqrt(UINT_MAX));
}

Field::Between Field::between(float coordinate, unsigned size)
{
	const float floored = std::floor(coordinate);
	const int before = static_cast<int>(floored) % static_cast<int>(size);
	const auto wrapped = static_cast<unsigned>(before < 0 ? before + static_cast<int>(size) : before);
	return {wrapped, (wrapped + 1) % size, coordinate - floored};
}

Field::Resampler::Resampler(const Field& field, unsigned size) :
	m_size(size),
	m_betweens(size),
	m_wideRows(std::size_t(field.m_size) * size)
{
	assert(size > 0);

	// A point's place between the grid points is the same all along its row and its column, so it is
	// worked out once per row and column; the field is square, so once for both.
	for (unsigned i = 0; i != size; ++i)
		m_betweens[i] = between((i + 0.5f) / size * field.m_size - 0.5f, field.m_size);

	// The blend across the columns, done once per row of the field rather than once per row of the grid.
	for (unsigned y = 0; y != field.m_size; ++y)
	{
		const auto values = std::span(field.m_values).subspan(std::size_t(y) * field.m_size, field.m_size);
		const auto wide = std::span(m_wideRows).subspan(std::size_t(y) * size, size);
		for (unsigned x = 0; x != size; ++x)
			wide[x] = std::lerp(values[m_betweens[x].before], values[m_betweens[x].after], m_betweens[x].fraction);
	}
}

void Field::Resampler::writeRow(unsigned y, std::span<float> row) const
{
	assert(row.size() == m_size);
	const Between& between = m_betweens[y];
	const auto before = std::span(m_wideRows).subspan(std::size_t(between.before) * m_size, m_size);
	const auto after = std::span(m_wideRows).subspan(std::size_t(between.after) * m_size, m_size);
	for (unsigned x = 0; x != m_size; ++x)
		row[x] = std::lerp(before[x], after[x], between.fraction);
}

void Field::stretchToUnit()
{
	const auto [min, max] = std::ranges::minmax(m_values);
	const float range = max - min;
	for (float& value : m_values)
		value = (range > 0) ? (value - min) / range : 0;
}
