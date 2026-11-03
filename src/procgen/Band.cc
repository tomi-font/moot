#include <moot/procgen/Band.hh>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

Band::Band(std::span<const Vector2f> points, float halfWidth)
{
	assert(!points.empty() && halfWidth > 0);
	for (std::size_t i = 1; i < points.size(); ++i)
		assert((points[i] - points[i - 1]).length() <= halfWidth); // The discs overlap into a band.

	// The rows the band covers.
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	for (const Vector2f& point : points)
	{
		minY = std::min(minY, point.y);
		maxY = std::max(maxY, point.y);
	}
	m_firstRow = static_cast<int>(std::floor(minY - halfWidth));
	const int lastRow = static_cast<int>(std::ceil(maxY + halfWidth));
	m_stretches.assign(lastRow - m_firstRow + 1, {std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()});

	// Each disc stretches the rows it crosses out to where it cuts them.
	for (const Vector2f& point : points)
	{
		for (int row = static_cast<int>(std::floor(point.y - halfWidth)); row <= static_cast<int>(std::ceil(point.y + halfWidth)); ++row)
		{
			const float dy = row + 0.5f - point.y;
			if (dy * dy >= halfWidth * halfWidth)
				continue;
			const float halfChord = std::sqrt(halfWidth * halfWidth - dy * dy); // Of the disc's cut across this row.
			Stretch& stretch = m_stretches[static_cast<std::size_t>(row - m_firstRow)];
			stretch.min = std::min(stretch.min, point.x - halfChord);
			stretch.max = std::max(stretch.max, point.x + halfChord);
		}
	}
}

void Band::appendRuns(Field& field, std::vector<Run>* runs) const
{
	const unsigned size = field.size();
	for (std::size_t i = 0; i != m_stretches.size(); ++i)
	{
		const Stretch& stretch = m_stretches[i];
		if (stretch.min > stretch.max)
			continue;
		const int y = m_firstRow + static_cast<int>(i);
		const std::size_t rowStart = std::size_t(field.wrap(y)) * size;
		const int start = static_cast<int>(std::ceil(stretch.min - 0.5f));
		const int end = static_cast<int>(std::floor(stretch.max - 0.5f)) + 1;
		assert(end - start <= static_cast<int>(size)); // A stretch wraps around at most once, so no cell gets two runs.
		for (int x = start; x < end;)
		{
			const unsigned first = field.wrap(x);
			const int count = std::min(end - x, static_cast<int>(size - first));
			runs->push_back({x, y, std::span(field.values()).subspan(rowStart + first, static_cast<std::size_t>(count))});
			x += count;
		}
	}
}
