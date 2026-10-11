#pragma once

#include <ranges>
#include <span>
#include <vector>
#include <SFML/System/Vector2.hpp>

class CConvexPolygon;

struct LightRay
{
	sf::Vector2f direction;
	float angle;
	float distance;
	const CConvexPolygon* hitPolygon;
};

// What a light reaches. A cache that SLight adds to every light and rewrites every frame.
class CLightVisibility
{
public:

	void setRays(const std::vector<LightRay>& rays);

	// The rays, sorted by angle.
	std::span<const LightRay> rays() const { return {m_rays.begin(), m_rays.size() / 2}; }

	using ClockwiseRays = std::ranges::subrange<std::vector<LightRay>::const_reverse_iterator>;
	// The clockwise rays from one angle to another, both within (-pi, pi].
	ClockwiseRays getRaysClockwise(float fromAngle, float toAngle) const;

private:

	std::vector<LightRay> m_rays;
};
