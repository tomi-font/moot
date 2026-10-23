#include <moot/Component/CMove.hh>
#include <cassert>

void CMove::setMotion(const sf::Vector2f& direction)
{
	// The direction's length is the share of the speed to move at, 1 at most.
	assert(direction.length() <= 1);

	m_velocity = direction * m_speed;
}
