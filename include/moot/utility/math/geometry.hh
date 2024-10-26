#pragma once

#include <moot/utility/math/base.hh>
#include <SFML/System/Vector2.hpp>

float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c);

static constexpr auto crossProductSign(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
{
	return normalize(crossProduct(a, b, c));
}
