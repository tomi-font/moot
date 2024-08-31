#include <moot/Component/CRender.hh>

CRender::CRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color)
{
	setPosition(pos, size);

	for (sf::Vertex& vertex : m_vertices)
		vertex.color = color;
}

void CRender::setPosition(const sf::Vector2f& pos, const sf::Vector2f& size)
{
	m_vertices[0].position = pos;
	m_vertices[1].position = {pos.x + size.x, pos.y};
	m_vertices[2].position = pos + size;
	m_vertices[3].position = {pos.x, pos.y + size.y};
}

void CRender::setPosition(const sf::Vector2f& pos)
{
	setPosition(pos, m_vertices[2].position - m_vertices[0].position);
}
