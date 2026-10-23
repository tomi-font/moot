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

	CCamera(const sf::Vector2f& size, const FloatRect& limits, float elevation = MaxElevation);

	void setSize(const sf::Vector2f&);
	void zoom(float factor);
	void setLimits(const FloatRect&);
	void setElevation(float radians);

	auto& size() const { return m_size; }
	auto& limits() const { return m_limits; }
	auto& elevation() const { return m_elevation; }

	bool seesExtrusions() const { return m_elevation != MaxElevation; }

	// Maps the world plane into the camera's frame, as seen from its elevation and the entity's rotation, y up.
	sf::Transform getTransform(float rotation) const;

private:

	void calculateNewSize();

	TrackedValue<Vector2f> m_size;
	TrackedValue<float> m_elevation;

	// The borders, in world coordinates, of the area this view is limited to show.
	FloatRect m_limits;
};
