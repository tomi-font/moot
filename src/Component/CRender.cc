#include <Component/Component.hh>
#include <Component/CRender.hh>

CRender::CRender(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::FloatRect& texRect) noexcept :
	m_vertices
	{
		sf::Vertex(pos, sf::Vector2f(texRect.left, texRect.top)),
		sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y), sf::Vector2f(texRect.left + texRect.width, texRect.top)),
		sf::Vertex(pos + size, sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height)),
		sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y), sf::Vector2f(texRect.left, texRect.top + texRect.height))
	}
{}

void	CRender::setPosition(const sf::Vector2f& pos) noexcept
{
	sf::Vector2f	size(m_vertices[2].position - m_vertices[0].position);

	m_vertices[0].position = pos;
	m_vertices[1].position = sf::Vector2f(pos.x + size.x, pos.y);
	m_vertices[2].position = pos + size;
	m_vertices[3].position = sf::Vector2f(pos.x, pos.y + size.y);
}
