#include <moot/Component/CMove.hh>
#include <moot/util/math/base.hh>
#include <cassert>

void CMove::setXMotion(int direction)
{
	assert(normalize(direction) == direction);
	m_velocity.x = m_speed * direction;
}
