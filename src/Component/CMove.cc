#include <moot/Component/CMove.hh>
#include <moot/util/math/base.hh>
#include <cassert>

void CMove::setXMotion(int direction)
{
	assert(normalize(direction) == direction);
	m_velocity.x = m_speed * direction;
}

void CMove::setMotion(const sf::Vector2f& direction)
{
	m_velocity = (direction == sf::Vector2f()) ? direction : direction.normalized() * float(m_speed);
}
