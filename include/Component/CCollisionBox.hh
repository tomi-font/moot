#pragma once

#include <utility/Rect.hh>

struct CCollisionBox : FloatRect
{
	CCollisionBox() = default;
	CCollisionBox(const Rect& rect) : Rect(rect) {}
	CCollisionBox(const sf::Vector2f& size) : Rect({}, size) {}
};
