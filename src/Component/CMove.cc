#include <moot/Component/CMove.hh>
#include <moot/util/math/base.hh>
#include <cassert>

void CMove::setMotion(const sf::Vector2f& direction)
{
	for (const float num : {direction.x, direction.y})
		assert(num == normalize(num));

	m_velocity = (direction == sf::Vector2f()) ? direction : direction.normalized() * m_speed;
}
