#include <moot/Component/CCamera.hh>
#include <moot/struct/Vector2.hh>
#include <algorithm>
#include <cassert>
#include <cmath>

CCamera::CCamera(const sf::Vector2f& size, const FloatRect& limits, float elevation, float rotation) :
	m_size(size),
	m_rotation(rotation),
	m_limits(limits)
{
	setElevation(elevation);
	calculateNewSize();
}

void CCamera::calculateNewSize()
{
	Vector2f viewSize = m_size;

	if (viewSize.min() <= 0)
		return;

	if (!m_limits.isEmpty())
	{
		// The ratio of the actual view size to the maximum size allowed.
		const Vector2f actualToMaxRatio = viewSize / m_limits.size;
		if (actualToMaxRatio.max() > 1)
		{
			const auto aspectRatio = viewSize.x / viewSize.y;

			if (actualToMaxRatio.x > actualToMaxRatio.y)
				viewSize = {m_limits.width, m_limits.width / aspectRatio};
			else
				viewSize = {m_limits.height * aspectRatio, m_limits.height};
		
			m_size = viewSize;
		}
	}
	assert(viewSize.min() > 0);
}

void CCamera::setSize(const sf::Vector2f& size)
{
	m_size = size;
	calculateNewSize();
}

void CCamera::zoom(float factor)
{
	assert(factor > 0);
	m_size.mut() *= factor;
	calculateNewSize();
}

void CCamera::setLimits(const FloatRect& limits)
{
	assert(limits.hasPositiveArea());
	m_limits = limits;
	calculateNewSize();
}

void CCamera::setElevation(float radians)
{
	m_elevation = std::clamp(radians, MinElevation, MaxElevation);
}

// Think of the screen as a window pane facing the camera. Looking straight down, the plane is parallel
// to the pane and lengths carry over as they are. Lower the camera and the plane tips away from the
// pane: a length across the view stays the same, a length going away from the camera shows shorter,
// by sin(elevation), down to nothing when looking along the plane. The rotation is the camera orbiting
// the plane, a turn around the vertical, so it comes first, on the plane itself.
sf::Transform CCamera::getGroundTransform() const
{
	// SFML post-multiplies: the rotation below is applied to points first, then the squash.
	sf::Transform transform;
	transform.scale({1, std::sin(m_elevation)});
	transform.rotate(sf::radians(m_rotation));
	return transform;
}
