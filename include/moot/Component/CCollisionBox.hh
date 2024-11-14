#pragma once

#include <moot/struct/Rect.hh>

struct CCollisionBox : FloatRect
{
	CCollisionBox() = default;
	CCollisionBox(const Rect& rect) : Rect(rect) {}
	CCollisionBox(const sf::Vector2f& boxSize) : Rect({}, boxSize) {}
};
