#include <System/SRender.hh>

SRender::SRender()
{
	m_groups.reserve(1);
	m_groups.emplace_back(C(Component::Transform) | C(Component::Render));
}

void	SRender::update()
{

}
