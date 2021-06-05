#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CCollisionBox : sf::FloatRect
{
	CCollisionBox(const sf::FloatRect& rect) noexcept : sf::FloatRect(rect) {}
};
