#include <moot/struct/BoundCoords.hh>

void BoundCoords::incorporate(const sf::Vector2f& point)
{
	min.x = std::min(min.x, point.x);
	min.y = std::min(min.y, point.y);
	max.x = std::max(max.x, point.x);
	max.y = std::max(max.y, point.y);
}

void BoundCoords::incorporate(const BoundCoords& other)
{
	min.x = std::min(min.x, other.min.x);
	min.y = std::min(min.y, other.min.y);
	max.x = std::max(max.x, other.max.x);
	max.y = std::max(max.y, other.max.y);
}
