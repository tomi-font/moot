#include <Component/CMove.hh>
#include <utility/math.hh>
#include <cassert>

void CMove::setMotionX(int direction)
{
	assert(normalize(direction) == direction);
	m_velocity.x = m_speed * direction;
}
