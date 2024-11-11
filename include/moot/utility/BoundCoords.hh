#pragma once

#include <moot/utility/Rect.hh>

struct BoundCoords
{
	BoundCoords() = default;
	BoundCoords(const sf::Vector2f& baseCoord) : min(baseCoord), max(baseCoord) {}

	bool isEmpty() const { return min == max; }

	FloatRect toRect() const { return {min, max - min}; }

	void incorporate(const BoundCoords& other);
	void incorporate(const sf::Vector2f& other) { incorporate(BoundCoords(other)); }

	void move(const sf::Vector2f& move)
	{
		min += move;
		max += move;
	}

	sf::Vector2f min;
	sf::Vector2f max;
};
