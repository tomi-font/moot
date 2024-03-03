#include <Component/CRender.hh>

CRender::CRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color) :
	m_vertices
	({
		{ pos, color },
		{ {pos.x + size.x, pos.y}, color },
		{ pos + size, color },
		{ {pos.x, pos.y + size.y}, color },
	})
{
}

void CRender::setPosition(const sf::Vector2f& pos)
{
	const sf::Vector2f size(m_vertices[2].position - m_vertices[0].position);

	m_vertices[0].position = pos;
	m_vertices[1].position = sf::Vector2f(pos.x + size.x, pos.y);
	m_vertices[2].position = pos + size;
	m_vertices[3].position = sf::Vector2f(pos.x, pos.y + size.y);
}

void CRender::resize(const sf::Vector2f& size)
{
	m_vertices[2].position = m_vertices[0].position + size;

	m_vertices[1].position.x = m_vertices[2].position.x;
	m_vertices[3].position.y = m_vertices[2].position.y;
}
