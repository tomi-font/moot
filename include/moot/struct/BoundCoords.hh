#pragma once

#include <moot/struct/Rect.hh>
#include <vector>

struct BoundCoords
{
	BoundCoords() = default;

	template<typename T> requires std::derived_from<T, sf::Vector2f>
	BoundCoords(const std::vector<T>& points)
	{
		for (const sf::Vector2f& point : points)
			incorporate(point);
	}

	operator FloatRect() const { return min.x <= max.x ? FloatRect(min, max - min) : FloatRect(); }

	void incorporate(const sf::Vector2f& point);
	void incorporate(const BoundCoords& other);

	void move(const sf::Vector2f& move)
	{
		min += move;
		max += move;
	}

	sf::Vector2f min = {INFINITY, INFINITY};
	sf::Vector2f max = {-INFINITY, -INFINITY};
};
