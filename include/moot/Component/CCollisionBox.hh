#pragma once

#include <moot/struct/Rect.hh>

struct CCollisionBox : FloatRect
{
	CCollisionBox(const Rect& rect) : Rect(rect) {}
};
