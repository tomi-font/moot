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

	BoundCoords& incorporate(const sf::Vector2f& point);
	BoundCoords& incorporate(const BoundCoords& other);
	BoundCoords& move(const sf::Vector2f& move);
	BoundCoords& extrude(const sf::Vector2f& extrusion);

	sf::Vector2f min = {INFINITY, INFINITY};
	sf::Vector2f max = {-INFINITY, -INFINITY};
};
