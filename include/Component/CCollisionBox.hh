#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CCollisionBox : sf::FloatRect
{
	using sf::FloatRect::Rect;

	void resize(const sf::Vector2f&);
};
