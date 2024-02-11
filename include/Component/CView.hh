#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CView
{
	// The area to which this view is constrained.
	const sf::FloatRect limits;
};
