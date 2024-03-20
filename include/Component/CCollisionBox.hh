#pragma once

#include <utility/Rect.hh>

struct CCollisionBox : FloatRect
{
	using FloatRect::Rect;
	CCollisionBox(const Rect& rect) : Rect(rect) {}
};
