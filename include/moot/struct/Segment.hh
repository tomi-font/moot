#pragma once

#include <SFML/System/Vector2.hpp>

struct Segment
{
	Segment(const sf::Vector2f& a, const sf::Vector2f& b) : a(a), vector(b - a) {}

	sf::Vector2f a;
	sf::Vector2f vector;
};
