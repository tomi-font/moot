#pragma once

#include <variant>

class Field;

// A soft wash: irregular patches of shade, from the largest down to the smallest.
class Wash
{
public:

	Wash(float maxPatchSize, float minPatchSize, float maxDarkness) :
		m_maxPatchSize(maxPatchSize), m_minPatchSize(minPatchSize), m_maxDarkness(maxDarkness) {}

	void paint(Field* tile, float texelsPerUnit, unsigned seed) const;

private:

	float m_maxPatchSize; // The world size of the largest patches.
	float m_minPatchSize; // The world size of the smallest patches.
	float m_maxDarkness;  // How dark the darkest patch gets, (0, 1].
};

// Loose bundles of short parallel arcs.
class Strokes
{
public:

	Strokes(float maxStrokeLength, float strokeWidth, float density, float maxDarkness) :
		m_maxStrokeLength(maxStrokeLength), m_strokeWidth(strokeWidth), m_density(density), m_maxDarkness(maxDarkness) {}

	void paint(Field* tile, float texelsPerUnit, unsigned seed) const;

private:

	float m_maxStrokeLength; // The world length of the longest stroke.
	float m_strokeWidth;     // The world width of a stroke.
	float m_density;         // Bundles of strokes per square of the longest stroke.
	float m_maxDarkness;     // How dark the darkest stroke gets, (0, 1].
};

// Round dots of varying size.
class Dots
{
public:

	Dots(float maxRadius, float density, float maxDarkness) :
		m_maxRadius(maxRadius), m_density(density), m_maxDarkness(maxDarkness) {}

	void paint(Field* tile, float texelsPerUnit, unsigned seed) const;

private:

	float m_maxRadius;   // The world radius of the largest dot.
	float m_density;     // Dots per square of the largest dot's diameter.
	float m_maxDarkness; // How dark the darkest dot gets, (0, 1].
};

class Layer
{
public:

	using Pattern = std::variant<Wash, Strokes, Dots>;

	Layer(Pattern pattern) : m_pattern(pattern) {}

	void paint(Field* tile, float texelsPerUnit, unsigned seed) const
	{
		std::visit([=](const auto& pattern) { pattern.paint(tile, texelsPerUnit, seed); }, m_pattern);
	}

private:

	Pattern m_pattern;
};
