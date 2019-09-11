#include <Component/CRender.hh>

CRender::CRender(const sf::Vector2f& pos, const sf::Vector2f& size) noexcept
{
	m_vertices[0].position = pos;
	m_vertices[1].position = sf::Vector2f(pos.x + size.x, pos.y);
	m_vertices[2].position = pos + size;
	m_vertices[3].position = sf::Vector2f(pos.x, pos.y + size.y);
}
