#pragma once

#include <SFML/System/Vector2.hpp>

struct CPosition : sf::Vector2f
{
	CPosition(const sf::Vector2f& pos) : sf::Vector2f(pos) {}
};
