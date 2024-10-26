#include <moot/utility/math/geometry.hh>

float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
{
	return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}
