#pragma once

#include <array>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

class CRender
{
public:

	CRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color);

	auto& vertices() const { return m_vertices; }
	sf::Vector2f getSize() const { return m_vertices[2].position - m_vertices[0].position; }

	void setPosition(const sf::Vector2f&);

protected:

	std::array<sf::Vertex, 4> m_vertices;

private:

	void setPosition(const sf::Vector2f&, const sf::Vector2f&);
};
