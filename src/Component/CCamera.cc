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

sf::Vector2f CCamera::shownPlaneSize(const sf::Vector2f& screenSize) const
{
	// The screen's corners land on the plane at plus and minus the images of its two half diagonals,
	// so the box is spanned, on each axis, by whichever of the two reaches farther.
	const sf::Transform toPlane = getGroundTransform().getInverse();
	const sf::Vector2f diagonal = toPlane.transformPoint(screenSize / 2.f);
	const sf::Vector2f antidiagonal = toPlane.transformPoint({screenSize.x / 2, -screenSize.y / 2});

	return {2 * std::max(std::abs(diagonal.x), std::abs(antidiagonal.x)),
	        2 * std::max(std::abs(diagonal.y), std::abs(antidiagonal.y))};
}

void CCamera::calculateNewSize()
{
	Vector2f viewSize = m_size;

	if (viewSize.min() <= 0)
		return; // Not sized yet (the size may be set after spawning).

	if (!m_limits.isEmpty())
	{
		// Shrink the screen, keeping its aspect ratio, until what it shows of the plane fits in the limits.
		const Vector2f shownToLimits = Vector2f(shownPlaneSize(m_size)) / m_limits.size;
		if (shownToLimits.max() > 1)
			m_size.mut() /= shownToLimits.max();
	}
	assert(Vector2f(m_size).min() > 0);
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
	calculateNewSize(); // The angles change how much plane the screen shows.
}

void CCamera::setRotation(float radians)
{
	m_rotation = radians;
	calculateNewSize();
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
