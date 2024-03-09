#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CCollisionBox : sf::FloatRect
{
	CCollisionBox(const sf::Vector2f& pos, const sf::Vector2f& size) : sf::FloatRect(pos.x, pos.y + size.y, size.x, size.y) {}
};
