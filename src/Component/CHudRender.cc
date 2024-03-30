#include <Component/CHudRender.hh>

CHudRender::CHudRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color) :
	CRender(pos, size, color)
{
}

void CHudRender::resize(const sf::Vector2f& size)
{
	m_vertices[2].position = m_vertices[0].position + size;

	m_vertices[1].position.x = m_vertices[2].position.x;
	m_vertices[3].position.y = m_vertices[2].position.y;
}
