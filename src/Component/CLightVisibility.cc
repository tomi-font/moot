#include <moot/Component/CLightVisibility.hh>
#include <moot/util/math/geometry.hh>
#include <algorithm>
#include <cassert>

void CLightVisibility::setRays(const std::vector<LightRay>& rays)
{
	m_rays = rays;

	// The rays once more a full turn later, so that iterating over the -pi/pi boundary is done without special case.
	for (const LightRay& ray : rays)
		m_rays.emplace_back(ray.direction, ray.angle + TwoPi, ray.distance, ray.hitPolygon);

	assert(std::ranges::is_sorted(m_rays, {}, &LightRay::angle));
}

CLightVisibility::ClockwiseRays CLightVisibility::getRaysClockwise(float fromAngle, float toAngle) const
{
	if (fromAngle < toAngle)
		fromAngle += TwoPi;
	const auto first = std::ranges::lower_bound(m_rays, toAngle, {}, &LightRay::angle);
	const auto pastLast = std::ranges::upper_bound(m_rays, fromAngle, {}, &LightRay::angle);
	return {std::make_reverse_iterator(pastLast), std::make_reverse_iterator(first)};
}
