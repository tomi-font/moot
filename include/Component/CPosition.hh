#pragma once

#include <SFML/System/Vector2.hpp>

struct CPosition : sf::Vector2f
{
public:

	CPosition(const sf::Vector2f& pos) noexcept : sf::Vector2f(pos) {}
};

