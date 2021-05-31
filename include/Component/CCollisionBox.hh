#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CCollisionBox : sf::FloatRect
{
	static constexpr auto	Type = Component::CollisionBox;

	CCollisionBox(const sf::FloatRect& rect) noexcept : sf::FloatRect(rect) {}
};
