#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Rect.hh>
#include <cmath>
#include <numbers>
#include <SFML/Graphics/Transform.hpp>

class CCamera
{
public:

	// How far the camera sees along the plane, in view heights: the light map covers that much, no more.
	static constexpr float MaxShownViewHeights = 10;

	// The angle between the ground and the line of sight, in radians: pi/2 looks straight at the world plane
	// (top-down or side view), smaller angles tilt the camera towards the horizon (oblique, isometric). A view
	// height shows 1 / sin(elevation) view heights of plane, so the camera stops where that reaches how far it
	// sees: lower, the far part of the screen would go unlit.
	inline static const float MinElevation = std::asin(1 / MaxShownViewHeights); // About 5.7 degrees.
	static constexpr float MaxElevation = std::numbers::pi_v<float> / 2;

	CCamera(const sf::Vector2f& size, const FloatRect& limits, float elevation, float rotation);

	void setSize(const sf::Vector2f&);
	void zoom(float factor);
	void setLimits(const FloatRect&);
	void setElevation(float radians);
	void setRotation(float radians);

	auto& size() const { return m_size; }
	auto& limits() const { return m_limits; }
	auto& elevation() const { return m_elevation; }
	auto& rotation() const { return m_rotation; }

	// Maps the world plane onto the screen as seen from the camera's elevation and rotation.
	sf::Transform getGroundTransform() const;
	// The size of the axis-aligned box of plane that a screen rectangle of the given size shows: the rectangle
	// itself when looking straight down, wider and deeper once the camera is rotated and tilted.
	sf::Vector2f shownPlaneSize(const sf::Vector2f& screenSize) const;

private:

	void calculateNewSize();

	TrackedValue<Vector2f> m_size;
	TrackedValue<float> m_elevation;
	TrackedValue<float> m_rotation;

	// The borders, in world coordinates, of the area this camera is limited to show.
	FloatRect m_limits;
};
