#pragma once

#include <moot/util/math/base.hh>
#include <SFML/System/Vector2.hpp>

static constexpr float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.y - a.y * b.x;
}

static constexpr float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
{
	return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}

static constexpr auto crossProductSign(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
{
	return normalize(crossProduct(a, b, c));
}
