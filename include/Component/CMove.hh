#pragma once

#include <SFML/System/Vector2.hpp>

// This component handles voluntary moving only.
struct CMove
{
	CMove(unsigned short speed) : speed(speed) {}

	const unsigned short speed;

	sf::Vector2f velocity;

	// When true, a non-zero velocity is assumed.
	bool moving = false;

	bool movedSinceLastUpdate = false;
};
