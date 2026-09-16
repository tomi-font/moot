#pragma once

#include <moot/struct/Vector2.hh>

// Where an entity stays, relative to its parent's position
struct CLocalPosition : Vector2f
{
	CLocalPosition(const sf::Vector2f& pos) : Vector2f(pos) {}
};
