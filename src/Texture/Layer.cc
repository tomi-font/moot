#include <moot/Texture/Layer.hh>
#include <moot/procgen/Band.hh>
#include <moot/procgen/Field.hh>
#include <moot/procgen/hash.hh>
#include <moot/procgen/Lattice.hh>
#include <moot/procgen/scatter.hh>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>
#include <span>
#include <vector>

void Wash::paint(Field* tile, float texelsPerUnit, unsigned seed) const
{
	assert(m_maxPatchSize >= m_minPatchSize && m_minPatchSize > 0 && m_maxDarkness > 0 && m_maxDarkness <= 1);

	// The wash is smooth where its patches are large, so those are worked out on a grid some texels apart and
	// blended over the tile; the patches too small for the grid are looked up per texel.
	constexpr unsigned GridStep = 4;
	const unsigned texelsAcross = tile->size();
	const unsigned stepsAcross = texelsAcross / GridStep;
	const float largestPatch = m_maxPatchSize * texelsPerUnit;                 // In texels.
	const float smallestPatch = std::max(2.f, m_minPatchSize * texelsPerUnit); // In texels; a patch is never under two.
	const auto cellsAcrossFor = [texelsAcross](float patch) { return static_cast<unsigned>(std::lround(texelsAcross / patch)); };
	const unsigned patchesAcross = cellsAcrossFor(largestPatch);
	assert(patchesAcross > 0 && patchesAcross * 2 <= stepsAcross);

	// Patches halve from octave to octave, from the largest down to the smallest, each octave at half the
	// weight of the one before: the ones at least two grid steps across go on the grid, the finer ones per texel.
	std::vector<Lattice> gridOctaves;
	std::vector<Lattice> texelOctaves;
	std::vector<float> texelWeights;
	float gridWeight = 0;
	float texelWeight = 0;
	float weight = 1;
	unsigned seedIndex = 2;
	for (float patch = largestPatch; patch >= smallestPatch; patch /= 2, weight /= 2, ++seedIndex)
	{
		if (patch >= 2.f * GridStep)
		{
			gridOctaves.emplace_back(cellsAcrossFor(patch), deriveSeed(seed, seedIndex));
			gridWeight += weight;
		}
		else
		{
			texelOctaves.emplace_back(cellsAcrossFor(patch), deriveSeed(seed, seedIndex));
			texelWeights.push_back(weight);
			texelWeight += weight;
		}
	}

	const Lattice warpX(patchesAcross, deriveSeed(seed, 0));
	const Lattice warpY(patchesAcross, deriveSeed(seed, 1));

	Field gridShade(stepsAcross);
	for (unsigned gridY = 0; gridY != stepsAcross; ++gridY)
	{
		for (unsigned gridX = 0; gridX != stepsAcross; ++gridX)
		{
			const float tileX = (gridX + 0.5f) / stepsAcross;
			const float tileY = (gridY + 0.5f) / stepsAcross;
			// The patches get their irregular shapes from being looked up
			// where other noise displaces the point, by up to half a patch.
			const float warpedX = tileX + (warpX.at(tileX, tileY) - 0.5f) / patchesAcross;
			const float warpedY = tileY + (warpY.at(tileX, tileY) - 0.5f) / patchesAcross;

			float sum = 0;
			float octaveWeight = 1;
			for (const Lattice& octave : gridOctaves)
			{
				sum += octaveWeight * octave.at(warpedX, warpedY);
				octaveWeight /= 2;
			}
			gridShade.at(gridX, gridY) = sum;
		}
	}
	gridShade.stretchToUnit();

	// The grid's shade is blended up to the tile a row at a time, the per-texel octaves are added, and it is multiplied in.
	const Field::Resampler tileShade(gridShade, texelsAcross);
	std::vector<float> shadeRow(texelsAcross);
	for (unsigned y = 0; y != texelsAcross; ++y)
	{
		tileShade.writeRow(y, shadeRow);
		const float tileY = (y + 0.5f) / texelsAcross;
		for (unsigned x = 0; x != texelsAcross; ++x)
		{
			const float tileX = (x + 0.5f) / texelsAcross;
			float sum = gridWeight * shadeRow[x];
			for (std::size_t i = 0; i != texelOctaves.size(); ++i)
				sum += texelWeights[i] * texelOctaves[i].at(tileX, tileY);
			tile->at(x, y) *= 1 - m_maxDarkness * sum / (gridWeight + texelWeight);
		}
	}
}

void Strokes::paint(Field* tile, float texelsPerUnit, unsigned seed) const
{
	assert(m_maxStrokeLength > 0 && m_strokeWidth > 0 && m_density > 0 && m_maxDarkness > 0 && m_maxDarkness <= 1);

	constexpr float BaseAngle = std::numbers::pi_v<float> / 9; // Leaning right, like most handwriting.
	constexpr float AngleSpread = std::numbers::pi_v<float> / 3.6f;
	constexpr float Spacing = 0.16f; // Between the arcs of a bundle, as a fraction of the longest stroke.
	constexpr unsigned MinArcs = 2;
	constexpr unsigned MaxArcs = 5;

	const unsigned texelsAcross = tile->size();
	const float longestStroke = m_maxStrokeLength * texelsPerUnit;
	const float halfWidth = std::max(0.5f, m_strokeWidth * texelsPerUnit / 2); // In texels; a stroke is never thinner than a texel.
	const float reach = halfWidth + 0.5f; // Where the coverage ends: the half width and the half texel of soft edge.
	const auto trait = [derivedSeed = deriveSeed(seed, 1)](unsigned bundle, unsigned index)
	{
		return hashToUnit(bundle, index, derivedSeed);
	};

	// The bundles spread evenly over the tile, as many as the density asks for its area.
	const auto bundlesAcross = static_cast<unsigned>(std::lround(std::sqrt(m_density * float(texelsAcross) * texelsAcross / (longestStroke * longestStroke))));
	assert(bundlesAcross > 0); // The tile holds a bundle at least.
	const std::vector<Vector2f> centres = jitteredGrid(bundlesAcross, deriveSeed(seed, 0));

	std::vector<Vector2f> points; // Of an arc, at most a texel apart.
	std::vector<Band::Run> runs;  // Of texels within reach of an arc.

	for (unsigned bundle = 0; bundle != centres.size(); ++bundle)
	{
		const sf::Vector2f centre = centres[bundle] * float(texelsAcross);
		const float angle = BaseAngle + (trait(bundle, 2) - 0.5f) * AngleSpread;
		const sf::Vector2f alongAxis = {std::cos(angle), std::sin(angle)};
		const sf::Vector2f acrossAxis = alongAxis.perpendicular();
		const float bend = (trait(bundle, 3) - 0.5f) * 1.2f;
		const auto arcCount = MinArcs + static_cast<unsigned>(trait(bundle, 4) * (MaxArcs - MinArcs + 1)) % (MaxArcs - MinArcs + 1);

		for (unsigned arc = 0; arc != arcCount; ++arc)
		{
			const float acrossOffset = (arc - (arcCount - 1) / 2.f + (trait(bundle, 10 + arc) - 0.5f) * 0.6f) * Spacing * longestStroke;
			const float arcLength = longestStroke * (0.33f + 0.67f * trait(bundle, 20 + arc));
			const float alongShift = (trait(bundle, 30 + arc) - 0.5f) * 0.4f * longestStroke;
			const float arcDarkness = m_maxDarkness * (0.4f + 0.6f * trait(bundle, 40 + arc));

			// In the stroke's frame, `along` down its axis and `across` over it, the arc is the parabola
			// along(t) = t * arcLength + alongShift, across(t) = acrossOffset + bow * (t^2 - 1/4) for t from -1/2 to 1/2:
			// its ends at across = acrossOffset, its middle a quarter of `bow` further across.
			const float bow = bend * arcLength;
			const float tPerTexel = 1 / arcLength;
			const auto arcPoint = [&](float t) { return centre + alongAxis * (t * arcLength + alongShift) + acrossAxis * (acrossOffset + bow * (t * t - 0.25f)); };

			// The parabola is steepest at its ends, where it runs sqrt(1 + bend^2) texels per texel along its axis;
			// points that dense along the axis are at most a texel apart, within the band's reach.
			const auto pointCount = static_cast<unsigned>(std::ceil(arcLength * std::sqrt(1 + bend * bend))) + 1;
			points.clear();
			for (unsigned i = 0; i != pointCount; ++i)
				points.emplace_back(arcPoint(float(i) / (pointCount - 1) - 0.5f));
			const Band band(points, reach);
			runs.clear();
			band.appendRuns(*tile, &runs);

			// Every texel within reach is inked once, by its distance to the arc: across it, corrected for the arc's
			// slope there, or to the nearer end for texels past the ends. A texel out of reach gets a factor of 1, so
			// the run has no branches to keep it from vectorising.
			for (const Band::Run& run : runs)
			{
				const std::span<float> texels = run.values;
				const auto texelCount = static_cast<int>(texels.size()); // Signed, so that float(i) vectorises.
				const float dy = run.y + 0.5f - centre.y; // The texel's centre from the bundle's.
				const float firstDx = run.x + 0.5f - centre.x;
				for (int i = 0; i != texelCount; ++i)
				{
					const float dx = firstDx + float(i);
					const float along = dx * alongAxis.x + dy * alongAxis.y;
					const float across = dx * acrossAxis.x + dy * acrossAxis.y;
					const float t = (along - alongShift) * tPerTexel;
					const float tOnArc = std::clamp(t, -0.5f, 0.5f);
					const float slope = 2 * bend * t;
					const float acrossDistance = std::fabs(across - (acrossOffset + bow * (t * t - 0.25f))) / std::sqrt(1 + slope * slope);
					const float toEndAlong = along - (tOnArc * arcLength + alongShift);
					const float toEndAcross = across - (acrossOffset + bow * (tOnArc * tOnArc - 0.25f));
					const float endDistance = std::sqrt(toEndAlong * toEndAlong + toEndAcross * toEndAcross);
					const float distance = t == tOnArc ? acrossDistance : endDistance;
					const float taper = 1 - 2.8f * tOnArc * tOnArc; // The ink fades towards the ends, to 0.3.
					texels[i] *= 1 - std::clamp(halfWidth + 0.5f - distance, 0.f, 1.f) * taper * arcDarkness;
				}
			}
		}
	}
}

void Dots::paint(Field* tile, float texelsPerUnit, unsigned seed) const
{
	assert(m_maxRadius > 0 && m_density > 0 && m_maxDarkness > 0 && m_maxDarkness <= 1);

	const unsigned texelsAcross = tile->size();
	const float largestRadius = m_maxRadius * texelsPerUnit; // In texels.
	const auto trait = [derivedSeed = deriveSeed(seed, 1)](unsigned dot, unsigned index)
	{
		return hashToUnit(dot, index, derivedSeed);
	};

	// The dots spread evenly over the tile, as many as the density asks for its area.
	const auto dotsAcross = static_cast<unsigned>(std::lround(std::sqrt(m_density * float(texelsAcross) * texelsAcross / (4 * largestRadius * largestRadius))));
	assert(dotsAcross > 0); // The tile holds a dot at least.
	const std::vector<Vector2f> centres = jitteredGrid(dotsAcross, deriveSeed(seed, 0));
	std::vector<Band::Run> runs; // Of texels within reach of a dot.

	for (unsigned dot = 0; dot != centres.size(); ++dot)
	{
		const Vector2f centre = centres[dot] * float(texelsAcross);
		const float radius = largestRadius * (0.4f + 0.6f * trait(dot, 0));
		const float darkness = m_maxDarkness * (0.5f + 0.5f * trait(dot, 1));
		const Band band(std::span(&centre, 1), radius + 0.5f);
		runs.clear();
		band.appendRuns(*tile, &runs);

		// Every texel within reach is inked once, by its distance to the centre, over a soft edge a texel wide.
		for (const Band::Run& run : runs)
		{
			const std::span<float> texels = run.values;
			const auto texelCount = static_cast<int>(texels.size()); // Signed, so that float(i) vectorises.
			const float dy = run.y + 0.5f - centre.y; // The texel's centre from the dot's.
			for (int i = 0; i != texelCount; ++i)
			{
				const float dx = run.x + i + 0.5f - centre.x;
				texels[i] *= 1 - std::clamp(radius + 0.5f - std::sqrt(dx * dx + dy * dy), 0.f, 1.f) * darkness;
			}
		}
	}
}
