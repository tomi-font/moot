#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Vector2.hh>

struct CTransform
{
	CTransform(const sf::Vector2f& position, float rotation = 0) : position(position), rotation(rotation) {}

	TrackedValue<Vector2f> position;
	TrackedValue<float> rotation; // Counter-clockwise radians.
};
