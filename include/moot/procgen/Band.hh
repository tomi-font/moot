#pragma once

#include <moot/struct/Vector2.hh>
#include <moot/procgen/Field.hh>
#include <span>
#include <vector>

// The cells within `halfWidth` of a polyline's points, as one stretch per row: discs around the points.
class Band
{
public:

	Band(std::span<const Vector2f> points, float halfWidth);

	// A run of consecutive cells along a row of a field: their values, and the unwrapped coordinates of the first
	// cell, which lie outside the field when the run came from wrapping around its edge.
	struct Run
	{
		int x;
		int y;
		std::span<float> values;
	};
	// Appends to `runs` the runs of cells of `field` within the band, row by row, cut where they wrap around the field's edges.
	void appendRuns(Field& field, std::vector<Run>* runs) const;

private:

	struct Stretch
	{
		float min; // In cells; min > max on a row the band misses.
		float max;
	};

	int m_firstRow;
	std::vector<Stretch> m_stretches;
};
