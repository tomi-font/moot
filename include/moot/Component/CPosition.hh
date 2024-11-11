#pragma once

#include <moot/TrackedValue.hh>
#include <moot/utility/Vector2.hh>

struct CPosition : TrackedValue<Vector2f>
{
	explicit CPosition(const sf::Vector2f& pos) : TrackedValue<Vector2f>(pos) {}

	using TrackedValue<Vector2f>::operator=;
};
