#include <moot/Component/CHudRender.hh>

CHudRender::CHudRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color)
{
	m_vertices[0].position = pos;
	m_vertices[1].position = {pos.x + size.x, pos.y};
	m_vertices[2].position = pos + size;
	m_vertices[3].position = {pos.x, pos.y + size.y};

	for (sf::Vertex& vertex : m_vertices)
		vertex.color = color;
}

void CHudRender::resize(const Vector2f& size)
{
	m_vertices[2].position = m_vertices[0].position + size;

	m_vertices[1].position.x = m_vertices[2].position.x;
	m_vertices[3].position.y = m_vertices[2].position.y;
}
