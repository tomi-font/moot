#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Rect.hh>
#include <numbers>
#include <SFML/Graphics/Transform.hpp>

class CCamera
{
public:

	static constexpr float MinElevation = std::numbers::pi_v<float> / 180; // 1 degree
	static constexpr float MaxElevation = std::numbers::pi_v<float> / 2;

	CCamera(const sf::Vector2f& size, const FloatRect& limits, float elevation = MaxElevation, float rotation = 0);

	void setSize(const sf::Vector2f&);
	void zoom(float factor);
	void setLimits(const FloatRect&);
	void setElevation(float radians);
	void setRotation(float radians) { m_rotation = radians; }

	auto& size() const { return m_size; }
	auto& limits() const { return m_limits; }
	auto& elevation() const { return m_elevation; }
	auto& rotation() const { return m_rotation; }

	// Maps the world plane as seen from the camera's elevation and rotation.
	sf::Transform getGroundTransform() const;

private:

	void calculateNewSize();

	TrackedValue<Vector2f> m_size;
	TrackedValue<float> m_elevation;
	TrackedValue<float> m_rotation;

	// The borders, in world coordinates, of the area this view is limited to show.
	FloatRect m_limits;
};
