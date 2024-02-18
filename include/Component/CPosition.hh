#pragma once

#include <SFML/System/Vector2.hpp>

struct CPosition : sf::Vector2f
{
	using sf::Vector2f::Vector2;
	CPosition(const sf::Vector2f& pos) : sf::Vector2f(pos) {}
};
