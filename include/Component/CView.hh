#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CView
{
	CView(const sf::FloatRect& limits) : limits(limits) {}

	// The area to which this view is constrained.
	const sf::FloatRect limits;
};
