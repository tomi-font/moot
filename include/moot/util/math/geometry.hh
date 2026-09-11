#pragma once

#include <moot/util/math/base.hh>
#include <SFML/System/Vector2.hpp>

static constexpr float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.y - a.y * b.x;
}

float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c);

static constexpr auto crossProductSign(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
{
	return normalize(crossProduct(a, b, c));
}
