#include <moot/struct/BoundCoords.hh>

BoundCoords& BoundCoords::incorporate(const sf::Vector2f& point)
{
	min.x = std::min(min.x, point.x);
	min.y = std::min(min.y, point.y);
	max.x = std::max(max.x, point.x);
	max.y = std::max(max.y, point.y);
	return *this;
}

BoundCoords& BoundCoords::incorporate(const BoundCoords& other)
{
	min.x = std::min(min.x, other.min.x);
	min.y = std::min(min.y, other.min.y);
	max.x = std::max(max.x, other.max.x);
	max.y = std::max(max.y, other.max.y);
	return *this;
}

BoundCoords& BoundCoords::move(const sf::Vector2f& move)
{
	min += move;
	max += move;
	return *this;
}

BoundCoords& BoundCoords::extrude(const sf::Vector2f& extrusion)
{
	min += {std::min(extrusion.x, 0.f), std::min(extrusion.y, 0.f)};
	max += {std::max(extrusion.x, 0.f), std::max(extrusion.y, 0.f)};
	return *this;
}
