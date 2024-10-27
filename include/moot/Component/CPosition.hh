#pragma once

#include <moot/TrackedValue.hh>
#include <SFML/System/Vector2.hpp>

struct CPosition : TrackedValue<sf::Vector2f>
{
	explicit CPosition(const sf::Vector2f& pos) : TrackedValue<sf::Vector2f>(pos) {}

	using TrackedValue<sf::Vector2f>::operator=;
};
